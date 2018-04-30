#include "TiledForwardRenderer.hpp"

#include "GLRenderingDevice.hpp"
#include "GLMeshDeviceProxy.hpp"
#include "RenderingPassBase.hpp"
#include "GLShaderProgram.hpp"

#include <World.hpp>
#include <BasicMath.hpp>
#include <AARect.hpp>
#include <CameraComponent.hpp>
#include <LightSourceComponent.hpp>
#include <MeshRenderingComponent.hpp>
#include <TimeWorldComponent.hpp>

using namespace Poly;

TiledForwardRenderer::TiledForwardRenderer(GLRenderingDevice* RenderingDeviceInterface)
	: IRendererInterface(RenderingDeviceInterface),
	depthShader("Shaders/depth.vert.glsl", "Shaders/depth.frag.glsl"),
	lightCullingShader("Shaders/light_culling.comp.glsl"),
	lightAccumulationShader("Shaders/light_accumulation.vert.glsl", "Shaders/light_accumulation.frag.glsl"),
	hdrShader("Shaders/hdr.vert.glsl", "Shaders/hdr.frag.glsl"),
	depthDebugShader("Shaders/depth_debug.vert.glsl", "Shaders/depth_debug.frag.glsl"),
	lightDebugShader("Shaders/light_debug.vert.glsl", "Shaders/light_debug.frag.glsl"),
	debugQuadDepthPrepassShader("Shaders/debugQuadDepthPrepass.vert.glsl", "Shaders/debugQuadDepthPrepass.frag.glsl"),
	debugQuadLightCullingShader("Shaders/debugQuadLightCulling.vert.glsl", "Shaders/debugQuadLightCulling.frag.glsl"),
	computeDebugShader("Shaders/test.comp.glsl"),
	debugLightAccumShader("Shaders/debugLightAccum.vert.glsl", "Shaders/debugLightAccum.frag.glsl")
{
}

void TiledForwardRenderer::Init()
{
	gConsole.LogInfo("TiledForwardRenderer::Init");

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	// glEnable(GL_MULTISAMPLE);

	gConsole.LogInfo("TiledForwardRenderer::Init SCREEN_SIZE: ({},{})", SCREEN_SIZE_X, SCREEN_SIZE_Y);

	// So we need to create a depth map FBO
	// This will be used in the depth pass
	// Create a depth map frame buffer object and texture
	glGenFramebuffers(1, &depthMapFBO);

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCREEN_SIZE_X, SCREEN_SIZE_Y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// Create a floating point HDR frame buffer and a floating point color buffer (as a texture)
	GLuint hdrFBO;
	glGenFramebuffers(1, &hdrFBO);

	GLuint colorBuffer;
	glGenTextures(1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_SIZE_X, SCREEN_SIZE_Y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// It will also need a depth component as a render buffer, attached to the hdrFBO
	GLuint rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_SIZE_X, SCREEN_SIZE_Y);

	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// Define work group sizes in x and y direction based off screen size and tile size (in pixels)
	workGroupsX = (SCREEN_SIZE_X + (SCREEN_SIZE_X % 16)) / 16;
	workGroupsY = (SCREEN_SIZE_Y + (SCREEN_SIZE_Y % 16)) / 16;
	size_t numberOfTiles = workGroupsX * workGroupsY;
	gConsole.LogInfo("TiledForwardRenderer::Init workGroups: ({},{}), numberOfTiles: {}", workGroupsX, workGroupsY, numberOfTiles);

	// Generate our shader storage buffers
	glGenBuffers(1, &lightBuffer);
	glGenBuffers(1, &visibleLightIndicesBuffer);
	glGenBuffers(1, &inputBuffer);
	glGenBuffers(1, &outputBuffer);

	// Bind light buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Light) * NUM_LIGHTS, 0, GL_DYNAMIC_DRAW);

	// Bind visible light indices buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, visibleLightIndicesBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(VisibleIndex) * NUM_LIGHTS * numberOfTiles, 0, GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, inputBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Input) * 16, 0, GL_STATIC_DRAW);

	// for test compute shader
	// glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputBuffer);
	// glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Output) * 16 * 4, 0, GL_STATIC_DRAW);
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Output) * numberOfTiles, 0, GL_STATIC_DRAW);

	SetupLightsBuffer();

	SetupInputBuffer();

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// lightCullingShader.BindProgram();
	// lightDebugShader.BindProgram();
	// lightDebugShader.SetUniform("totalLightCount", (int)NUM_LIGHTS);
	// lightDebugShader.SetUniform("numberOfTilesX", (int)workGroupsX);
}

void TiledForwardRenderer::Deinit()
{
	gConsole.LogInfo("TiledForwardRenderer::Deinit");
}

void TiledForwardRenderer::Render(World* world, const AARect& rect, const CameraComponent* cameraCmp)
{
	gConsole.LogInfo("TiledForwardRenderer::Render");

	UpdateLights(world);

	// ComputeTest(world, cameraCmp);

	DepthPrePass(world, cameraCmp);

	LightCulling(world, cameraCmp);

	// DrawDepthPrepass(cameraCmp);

	// DrawLightCulling(cameraCmp);

	DrawLightAccum(world, cameraCmp);

	// DebugLightCulling(world, cameraCmp);

	// DebugDepth(world, cameraCmp);

	// glDepthMask(GL_TRUE);
	// glEnable(GL_DEPTH_TEST);
	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_BACK);
	// 
	// RDI->GeometryRenderingPasses[GLRenderingDevice::eGeometryRenderPassType::BLINN_PHONG]->Run(world, cameraCmp, rect);
	// RDI->GeometryRenderingPasses[GLRenderingDevice::eGeometryRenderPassType::UNLIT]->Run(world, cameraCmp, rect);
	// 
	// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// glDisable(GL_BLEND);
	// glDisable(GL_DEPTH_TEST);
	// 
	// RDI->PostprocessRenderingPasses[GLRenderingDevice::ePostprocessRenderPassType::FOREGROUND_LIGHT]->Run(world, cameraCmp, rect);
}

void TiledForwardRenderer::DepthPrePass(World* world, const CameraComponent* cameraCmp)
{
	// Step 1: Render the depth of the scene to a depth map
	depthShader.BindProgram();
	// glUniformMatrix4fv(glGetUniformLocation(depthShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	// glUniformMatrix4fv(glGetUniformLocation(depthShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

	// Bind the depth map's frame buffer and draw the depth map to it
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	const Matrix& ScreenFromWorld = cameraCmp->GetClipFromWorld();
	for (auto componentsTuple : world->IterateComponents<MeshRenderingComponent>())
	{
		const MeshRenderingComponent* meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);
		const EntityTransform& transform = meshCmp->GetTransform();
		const Matrix& WorldFromModel = transform.GetWorldFromModel();
		depthShader.SetUniform("uScreenFromModel", ScreenFromWorld * WorldFromModel);

		for (const MeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
		{
			const GLMeshDeviceProxy* meshProxy = static_cast<const GLMeshDeviceProxy*>(subMesh->GetMeshProxy());
			glBindVertexArray(meshProxy->GetVAO());

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, FallbackWhiteTexture);

			glDrawElements(GL_TRIANGLES, (GLsizei)subMesh->GetMeshData().GetTriangleCount() * 3, GL_UNSIGNED_INT, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindVertexArray(0);
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TiledForwardRenderer::DrawDepthPrepass(const CameraComponent* cameraCmp)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Weirdly, moving this call drops performance into the floor
	debugQuadDepthPrepassShader.BindProgram();
	debugQuadDepthPrepassShader.SetUniform("near", cameraCmp->GetClippingPlaneNear());
	debugQuadDepthPrepassShader.SetUniform("far", cameraCmp->GetClippingPlaneFar());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	DrawQuad();
}

void TiledForwardRenderer::LightCulling(World* world, const CameraComponent* cameraCmp)
{
	float Time = (float)(world->GetWorldComponent<TimeWorldComponent>()->GetGameplayTime());
	
	gConsole.LogInfo("TiledForwardRenderer::LightCulling Time: {}, workGroupsX: {}, workGroupsY: {}",
		Time, workGroupsX, workGroupsY
	);

	// Step 2: Perform light culling on point lights in the scene
	lightCullingShader.BindProgram();
	lightCullingShader.SetUniform("time", Time);
	lightCullingShader.SetUniform("near", cameraCmp->GetClippingPlaneNear());
	lightCullingShader.SetUniform("far", cameraCmp->GetClippingPlaneFar());
	lightCullingShader.SetUniform("ViewFromWorld", cameraCmp->GetViewFromWorld());
	lightCullingShader.SetUniform("ClipFromWorld", cameraCmp->GetClipFromWorld());
	lightCullingShader.SetUniform("ClipFromView", cameraCmp->GetClipFromView());
	lightCullingShader.SetUniform("lightCount", (int)NUM_LIGHTS);
	lightCullingShader.SetUniform("screenSizeX", (int)SCREEN_SIZE_X);
	lightCullingShader.SetUniform("screenSizeY", (int)SCREEN_SIZE_Y);
	lightCullingShader.SetUniform("workGroupsX", (int)workGroupsX);
	lightCullingShader.SetUniform("workGroupsY", (int)workGroupsY);
	lightCullingShader.SetUniform("lightCount", (int)NUM_LIGHTS);

	// Bind depth map texture to texture location 4 (which will not be used by any model texture)
	glActiveTexture(GL_TEXTURE4);
	glUniform1i(glGetUniformLocation(lightCullingShader.GetProgramHandle(), "depthMap"), 4);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	// Bind shader storage buffer objects for the light and indice buffers
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, outputBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, visibleLightIndicesBuffer);

	// Dispatch the compute shader, using the workgroup values calculated earlier
	glDispatchCompute(workGroupsX, workGroupsY, 1);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, 0);

	// Unbind the depth map
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, 0);

	// debug
	// gConsole.LogInfo("TiledForwardRenderer::LightCulling output");
	// glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputBuffer);
	// Output *outputBufferMapping = (Output*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);
	// 
	// size_t numberOfTiles = workGroupsX * workGroupsY;
	// for (int i = 0; i < numberOfTiles; ++i) {
	// 	Output &output = outputBufferMapping[i];
	// 	gConsole.LogInfo("TiledForwardRenderer::LightCulling #{}: indexGlobal: {}, indexWorkGroup: {}, indexLocal: {}, input: {}, result: {}",
	// 		i, output.indexGlobal, output.indexWorkGroup, output.indexLocal, output.input, output.result
	// 	);
	// }
	// 
	// glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	// glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void TiledForwardRenderer::DrawLightCulling(const CameraComponent* cameraCmp)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Weirdly, moving this call drops performance into the floor
	debugQuadLightCullingShader.BindProgram();
	debugQuadLightCullingShader.SetUniform("workGroupsX", (int)workGroupsX);
	debugQuadLightCullingShader.SetUniform("workGroupsY", (int)workGroupsY);
	debugQuadLightCullingShader.SetUniform("near", cameraCmp->GetClippingPlaneNear());
	debugQuadLightCullingShader.SetUniform("far", cameraCmp->GetClippingPlaneFar());
	debugQuadLightCullingShader.SetUniform("ClipFromWorld", cameraCmp->GetClipFromWorld());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, outputBuffer);
	DrawQuad();
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void TiledForwardRenderer::DebugLightCulling(World* world, const CameraComponent* cameraCmp)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	lightDebugShader.BindProgram();
	lightDebugShader.SetUniform("near", cameraCmp->GetClippingPlaneNear());
	lightDebugShader.SetUniform("far", cameraCmp->GetClippingPlaneFar());
	// lightDebugShader.SetUniform("projection", cameraCmp->GetProjectionMatrix());
	// lightDebugShader.SetUniform("view", cameraCmp->GetModelViewMatrix());
	// lightDebugShader.SetUniform("viewPosition", cameraCmp->GetOwner()->GetTransform().GetGlobalTranslation());

	const Matrix& ClipFromWorld = cameraCmp->GetClipFromWorld();
	for (auto componentsTuple : world->IterateComponents<MeshRenderingComponent>())
	{
		const MeshRenderingComponent* meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);
		const EntityTransform& transform = meshCmp->GetTransform();
		const Matrix& WorldFromModel = transform.GetWorldFromModel();
		lightDebugShader.SetUniform("uScreenFromModel", ClipFromWorld * WorldFromModel);

		for (const MeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
		{
			const GLMeshDeviceProxy* meshProxy = static_cast<const GLMeshDeviceProxy*>(subMesh->GetMeshProxy());
			glBindVertexArray(meshProxy->GetVAO());

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, FallbackWhiteTexture);

			glDrawElements(GL_TRIANGLES, (GLsizei)subMesh->GetMeshData().GetTriangleCount() * 3, GL_UNSIGNED_INT, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindVertexArray(0);
		}
	}

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, 0);
}

void TiledForwardRenderer::DebugDepth(World* world, const CameraComponent* cameraCmp)
{
	depthDebugShader.BindProgram();
	depthDebugShader.SetUniform("near", cameraCmp->GetClippingPlaneNear());
	depthDebugShader.SetUniform("far", cameraCmp->GetClippingPlaneFar());

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const Matrix& ScreenFromWorld = cameraCmp->GetClipFromWorld();
	for (auto componentsTuple : world->IterateComponents<MeshRenderingComponent>())
	{
		const MeshRenderingComponent* meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);
		const EntityTransform& transform = meshCmp->GetTransform();
		const Matrix& WorldFromModel = transform.GetWorldFromModel();
		depthDebugShader.SetUniform("uScreenFromModel", ScreenFromWorld * WorldFromModel);

		for (const MeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
		{
			const GLMeshDeviceProxy* meshProxy = static_cast<const GLMeshDeviceProxy*>(subMesh->GetMeshProxy());
			glBindVertexArray(meshProxy->GetVAO());

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, FallbackWhiteTexture);

			glDrawElements(GL_TRIANGLES, (GLsizei)subMesh->GetMeshData().GetTriangleCount() * 3, GL_UNSIGNED_INT, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindVertexArray(0);
		}
	}
}

void TiledForwardRenderer::DrawLightAccum(World* world, const CameraComponent* cameraCmp)
{
	float Time = (float)(world->GetWorldComponent<TimeWorldComponent>()->GetGameplayTime());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	debugLightAccumShader.BindProgram();
	debugLightAccumShader.SetUniform("time", Time);
	debugLightAccumShader.SetUniform("workGroupsX", (int)workGroupsX);
	debugLightAccumShader.SetUniform("workGroupsY", (int)workGroupsY);
	debugLightAccumShader.SetUniform("lightCount", (int)NUM_LIGHTS);
	 
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, outputBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, visibleLightIndicesBuffer);
	 
	// Bind the depth map's frame buffer and draw the depth map to it
	// glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	// glClear(GL_DEPTH_BUFFER_BIT);
	
	const Matrix& ClipFromWorld = cameraCmp->GetClipFromWorld();
	for (auto componentsTuple : world->IterateComponents<MeshRenderingComponent>())
	{
		const MeshRenderingComponent* meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);
		const EntityTransform& transform = meshCmp->GetTransform();
		const Matrix& WorldFromModel = transform.GetWorldFromModel();
		debugLightAccumShader.SetUniform("uClipFromModel", ClipFromWorld * WorldFromModel);
		debugLightAccumShader.SetUniform("uWorldFromModel", WorldFromModel);
	
		for (const MeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
		{
			const GLMeshDeviceProxy* meshProxy = static_cast<const GLMeshDeviceProxy*>(subMesh->GetMeshProxy());
			glBindVertexArray(meshProxy->GetVAO());
	
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, FallbackWhiteTexture);
	
			glDrawElements(GL_TRIANGLES, (GLsizei)subMesh->GetMeshData().GetTriangleCount() * 3, GL_UNSIGNED_INT, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindVertexArray(0);
		}
	}
	 
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, 0);
}

void TiledForwardRenderer::SetupLightsBuffer()
{
	gConsole.LogInfo("TiledForwardRenderer::SetupLightsBuffer");

	if (lightBuffer == 0)
	{
		return;
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightBuffer);
	Light *lights = (Light*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

	LightsStartPositions.Reserve(NUM_LIGHTS);

	for (int i = 0; i < NUM_LIGHTS; ++i)
	{
		Light &light = lights[i];
		// light.Position = Vector(10.0f * i, 0.0f, 0.0f);
		Vector position = Vector(Random(-1.0f, 1.0f)*10.0f, 0.0f, Random(-1.0f, 1.0f)*10.0f);
		light.Position = position;
		light.Radius = 10.0f;
	
		LightsStartPositions.PushBack(position);

		gConsole.LogInfo("TiledForwardRenderer::SetupLights #{}: Positon: {}, Radius: {}",
			i, light.Position, light.Radius);
	}

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void TiledForwardRenderer::UpdateLights(World* world)
{
	gConsole.LogInfo("TiledForwardRenderer::UpdateLights");

	float Time = (float)(world->GetWorldComponent<TimeWorldComponent>()->GetGameplayTime());

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightBuffer);
	Light *lights = (Light*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

	for (int i = 0; i < NUM_LIGHTS; ++i)
	{
		Vector StartPosition = LightsStartPositions[i];
		float t = Time + StartPosition.X;
		float s = Sin(1.0_rad * t);
		float c = Cos(1.0_rad * t);
		Vector Offset = Vector(c, 0.0f, s) * 10.0f;
		Light &light = lights[i];
		light.Position = StartPosition + Offset;
	}

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void TiledForwardRenderer::DrawQuad() {
	if (quadVAO == 0) {
		GLfloat quadVertices[] = {
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};

		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void TiledForwardRenderer::CreateFallbackWhiteTexture()
{
	glGenTextures(1, &FallbackWhiteTexture);

	GLubyte data[] = { 255, 255, 255, 255 };

	glBindTexture(GL_TEXTURE_2D, FallbackWhiteTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
}

#pragma region Compute Test

void TiledForwardRenderer::SetupInputBuffer()
{
	ASSERTE(inputBuffer != 0, "TiledForwardRenderer::SetupInputBuffer inputBuffer is null");
	ASSERTE(outputBuffer != 0, "TiledForwardRenderer::SetupInputBuffer outputBuffer is null");

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, inputBuffer);
	Input *inputBufferMapping = (Input*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

	for (int i = 0; i < 16; i++) {
		Input &input = inputBufferMapping[i];
		input.value = i;
		
		gConsole.LogInfo("TiledForwardRenderer::SetupInputBuffer #{}: {}", i, input.value);
	}

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void TiledForwardRenderer::ComputeTest(World* world, const CameraComponent* cameraCmp)
{
	gConsole.LogInfo("TiledForwardRenderer::ComputeTest");

	ASSERTE(inputBuffer != 0, "TiledForwardRenderer::ComputeTest inputBuffer is null");
	ASSERTE(outputBuffer != 0, "TiledForwardRenderer::ComputeTest outputBuffer is null");

	computeDebugShader.BindProgram();

	// Bind depth map texture to texture location 4 (which will not be used by any model texture)
	glActiveTexture(GL_TEXTURE4);
	glUniform1i(glGetUniformLocation(computeDebugShader.GetProgramHandle(), "depthMap"), 4);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	// Bind shader storage buffer objects for the light and indice buffers
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, inputBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, outputBuffer);

	// Dispatch the compute shader, using the workgroup values calculated earlier
	glDispatchCompute(4, 4, 1);

	// Unbind the depth map
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);

	gConsole.LogInfo("TiledForwardRenderer::ComputeTest output");
	// debug
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputBuffer);
	Output *outputBufferMapping = (Output*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

	for (int i = 0; i < 16 * 4; i++) {
		Output &output = outputBufferMapping[i];
		gConsole.LogInfo("TiledForwardRenderer::ComputeTest #{}: indexGlobal: {}, indexWorkGroup: {}, indexLocal: {}, input: {}, result: {}",
			i, output.indexGlobal, output.indexWorkGroup, output.indexLocal, output.input, output.result
		);
	}

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

#pragma region end
