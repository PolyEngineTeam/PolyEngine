#include "TiledForwardRenderer.hpp"

#include "GLRenderingDevice.hpp"
#include "GLMeshDeviceProxy.hpp"
#include "RenderingPassBase.hpp"

#include <World.hpp>
#include <AARect.hpp>
#include <CameraComponent.hpp>
#include <LightSourceComponent.hpp>
#include <TransformComponent.hpp>
#include <MeshRenderingComponent.hpp>

using namespace Poly;

TiledForwardRenderer::TiledForwardRenderer(GLRenderingDevice* RenderingDeviceInterface)
	: IRendererInterface(RenderingDeviceInterface),
	depthShader("Shaders/depth.vert.glsl", "Shaders/depth.frag.glsl"),
	lightCullingShader("Shaders/light_culling.comp.glsl"),
	lightAccumulationShader("Shaders/light_accumulation.vert.glsl", "Shaders/light_accumulation.frag.glsl"),
	hdrShader("Shaders/hdr.vert.glsl", "Shaders/hdr.frag.glsl"),
	depthDebugShader("Shaders/depth_debug.vert.glsl", "Shaders/depth_debug.frag.glsl"),
	lightDebugShader("Shaders/light_debug.vert.glsl", "Shaders/light_debug.frag.glsl")
{
	gConsole.LogInfo("TiledForwardRenderer::TiledForwardRenderer");
}

void TiledForwardRenderer::Init()
{
	gConsole.LogInfo("TiledForwardRenderer::Init");

	ASSERTE(&depthShader != nullptr, "depthShader is NULL");
	ASSERTE(&hdrShader != nullptr, "hdrShader is NULL");
	
	depthShader.RegisterUniform("mat4", "uMVPTransform");
	hdrShader.RegisterUniform("float", "near");
	hdrShader.RegisterUniform("float", "far");
	hdrShader.RegisterUniform("float", "numberOfTilesX");
	hdrShader.RegisterUniform("float", "totalLightCount");
	// lightCullingShader.RegisterUniform("sampler2", "depthMap");
	lightAccumulationShader.RegisterUniform("vec4", "viewPosition");

	CreateDummyTexture();

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


	glGenFramebuffers(1, &hdrFBO);

	glGenTextures(1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_SIZE_X, SCREEN_SIZE_Y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

	glGenBuffers(1, &lightBuffer);
	glGenBuffers(1, &visibleLightIndicesBuffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_LIGHTS * sizeof(PointLight), 0, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, visibleLightIndicesBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, numberOfTiles * sizeof(VisibleIndex) * 1024, 0, GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// For each shader, bind the uniforms that will during the program's execution
	// depthShader.BindProgram();
	// glUniformMatrix4fv(glGetUniformLocation(depthShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// lightCullingShader.BindProgram();
	// glUniform1i(glGetUniformLocation(lightCullingShader.Program, "lightCount"), NUM_LIGHTS);
	// glUniform2iv(glGetUniformLocation(lightCullingShader.Program, "screenSize"), 1, &SCREEN_SIZE[0]);

	// lightAccumulationShader.Use();
	// glUniformMatrix4fv(glGetUniformLocation(lightAccumulationShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	// glUniform1i(glGetUniformLocation(lightAccumulationShader.Program, "numberOfTilesX"), workGroupsX);
}

void TiledForwardRenderer::Deinit()
{
	gConsole.LogInfo("TiledForwardRenderer::Deinit");
}

void TiledForwardRenderer::Render(World* world, const AARect& rect, CameraComponent* cameraCmp)
{
	const eRenderingModeType renderingMode = cameraCmp->GetRenderingMode();

	switch (renderingMode)
	{
		case eRenderingModeType::LIT:
  			RenderLit(world, rect, cameraCmp);
 			break;

		default:
			ASSERTE(false, "Uknown eRenderingModeType");
	}
}

void TiledForwardRenderer::RenderLit(World* world, const AARect& rect, CameraComponent* cameraCmp)
{
	gConsole.LogInfo("TiledForwardRenderer::RenderLit");

	const TransformComponent* cameraTransCmp = cameraCmp->GetSibling<TransformComponent>();
	const Matrix& mvp = cameraCmp->GetMVP();
	
	//============================================================================
	// TILED FORWARD
	//============================================================================

	UpdateLights(world);

	// Step 1: Render the depth of the scene to a depth map
	depthShader.BindProgram();
	// glUniformMatrix4fv(glGetUniformLocation(depthShader.Program, "uMVPTransform"), 1, GL_FALSE, mvp);
	// glUniformMatrix4fv(glGetUniformLocation(depthShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	// depthShader.SetUniform("uMVPTransform", mvp);

	// Bind the depth map's frame buffer and draw the depth map to it
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	DrawMeshes(world, depthShader, mvp);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// // Step 2: Perform light culling on point lights in the scene
	lightCullingShader.BindProgram();
	lightCullingShader.SetUniform("projection", cameraCmp->GetProjectionMatrix());
	lightCullingShader.SetUniform("view", cameraCmp->GetModelViewMatrix());
	// glUniformMatrix4fv(glGetUniformLocation(lightCullingShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	// glUniformMatrix4fv(glGetUniformLocation(lightCullingShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	
	// Bind depth map texture to texture location 4 (which will not be used by any model texture)
	glActiveTexture(GL_TEXTURE4);
	glUniform1i(glGetUniformLocation((GLuint)lightCullingShader.GetProgramHandle(), "depthMap"), 4);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	
	// Bind shader storage buffer objects for the light and indice buffers
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, visibleLightIndicesBuffer);
	
	// Dispatch the compute shader, using the workgroup values calculated earlier
	glDispatchCompute(workGroupsX, workGroupsY, 1);
	
	// Unbind the depth map
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, 0);

	// // Step 3: Light accumulation
	// // We render the scene into the floating point HDR frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	 
	// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	 
	// lightAccumulationShader.BindProgram();
	// lightAccumulationShader.SetUniform("viewPosition", cameraTransCmp->GetGlobalTranslation());
	// // glUniformMatrix4fv(glGetUniformLocation(lightAccumulationShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	// // glUniformMatrix4fv(glGetUniformLocation(lightAccumulationShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	//  
	// // sponzaModel.Draw(lightAccumulationShader);
	// DrawMeshes(world, lightAccumulationShader, mvp);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Tonemap the HDR colors to the default framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Weirdly, moving this call drops performance into the floor
	hdrShader.BindProgram();
	glActiveTexture(GL_TEXTURE0);
	// glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	// glUniform1f(glGetUniformLocation(hdrShader.Program, "exposure"), exposure);
	hdrShader.SetUniform("near", cameraCmp->GetNear());
	hdrShader.SetUniform("far", cameraCmp->GetFar());
	hdrShader.SetUniform("numberOfTilesX", (float)workGroupsX);
	hdrShader.SetUniform("totalLightCount", (float)NUM_LIGHTS);
	DrawQuad();

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);

	// //============================================================================
	// // FORWARD
	// //============================================================================
	// 
	// glDepthMask(GL_TRUE);
	// glEnable(GL_DEPTH_TEST);
	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_BACK);
	// 
	// // Render meshes with blin-phong shader
	// RDI->GeometryRenderingPasses[GLRenderingDevice::eGeometryRenderPassType::BLINN_PHONG]->Run(world, cameraCmp, rect);
	// 
	// // Render meshes with unlit shader
	// RDI->GeometryRenderingPasses[GLRenderingDevice::eGeometryRenderPassType::UNLIT]->Run(world, cameraCmp, rect);
}

void TiledForwardRenderer::DrawMeshes(World* world, GLShaderProgram& shader, const Matrix& mvp)
{
	gConsole.LogInfo("TiledForwardRenderer::DrawMeshes");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, WhiteDummy);
	for (auto componentsTuple : world->IterateComponents<MeshRenderingComponent, TransformComponent>())
	{
		const MeshRenderingComponent* meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);
		const TransformComponent* transCmp = std::get<TransformComponent*>(componentsTuple);
		const Matrix& ModelTransform = transCmp->GetGlobalTransformationMatrix();
		Matrix MVPTransfrom = mvp * ModelTransform;
		shader.SetUniform("uMVPTransform", MVPTransfrom);

		for (const MeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
		{
			const GLMeshDeviceProxy* meshProxy = static_cast<const GLMeshDeviceProxy*>(subMesh->GetMeshProxy());
			glBindVertexArray(meshProxy->GetVAO());
			glDrawElements(GL_TRIANGLES, (GLsizei)subMesh->GetMeshData().GetTriangleCount() * 3, GL_UNSIGNED_INT, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindVertexArray(0);
		}
	}
}

void TiledForwardRenderer::DrawQuad()
{
	gConsole.LogInfo("TiledForwardRenderer::DrawQuad");

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

void TiledForwardRenderer::UpdateLights(World* world) const
{
	gConsole.LogInfo("TiledForwardRenderer::UpdateLights");

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightBuffer);
	PointLight *pointLights = (PointLight*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

	unsigned int i = 0;
	for (auto componentsTuple : world->IterateComponents<PointLightComponent, TransformComponent>())
	{
		const PointLightComponent* pointLightCmp = std::get<PointLightComponent*>(componentsTuple);
		const TransformComponent* transCmp = std::get<TransformComponent*>(componentsTuple);

		PointLight &light = pointLights[i];
		light.position = transCmp->GetGlobalTranslation();
		Color c = pointLightCmp->GetColor();
		light.color = Vector(c.R, c.G, c.B);
		light.paddingAndRadius = Vector(0.0f, 0.0f, 0.0f, pointLightCmp->GetRange());

		gConsole.LogInfo("TiledForwardRenderer::UpdateLights[{}], p: {}, c: {}, r: {}",
			i, light.position, light.color, light.paddingAndRadius.W);

		if (i >= NUM_LIGHTS) break;
		++i;
	}

	while (i < NUM_LIGHTS)
	{
		PointLight &light = pointLights[i];
		light.position = Vector::ZERO;
		light.color = Vector::ZERO;
		light.paddingAndRadius = Vector(0.0f, 0.0f, 0.0f, 0.0f);
		++i;
	}

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void TiledForwardRenderer::CreateDummyTexture()
{
	gConsole.LogInfo("TiledForwardRenderer::CreateDummyTexture");

	glGenTextures(1, &WhiteDummy);

	GLubyte data[] = { 255, 255, 255, 255 };

	glBindTexture(GL_TEXTURE_2D, WhiteDummy);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
}
