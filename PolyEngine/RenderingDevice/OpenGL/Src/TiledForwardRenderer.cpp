#include "TiledForwardRenderer.hpp"

#include "GLRenderingDevice.hpp"
#include "Proxy/GLMeshDeviceProxy.hpp"
#include "Pipeline/RenderingPassBase.hpp"
#include "Common/GLShaderProgram.hpp"

#include <ECS/World.hpp>
#include <Math/BasicMath.hpp>
#include <Math/AARect.hpp>
#include <Rendering/Camera/CameraComponent.hpp>
#include <Rendering/Lighting/LightSourceComponent.hpp>
#include <Rendering/MeshRenderingComponent.hpp>
#include <Time/TimeWorldComponent.hpp>

using namespace Poly;

TiledForwardRenderer::TiledForwardRenderer(GLRenderingDevice* RenderingDeviceInterface)
	: IRendererInterface(RenderingDeviceInterface),
	depthShader("Shaders/depth.vert.glsl", "Shaders/depth.frag.glsl"),
	lightCullingShader("Shaders/light_culling.comp.glsl"),
	debugQuadDepthPrepassShader("Shaders/debugQuadDepthPrepass.vert.glsl", "Shaders/debugQuadDepthPrepass.frag.glsl"),
	debugLightAccumShader("Shaders/debugLightAccum.vert.glsl", "Shaders/debugLightAccum.frag.glsl"),
	lightAccumulationShader("Shaders/lightAccumulationVert.shader", "Shaders/lightAccumulationFrag.shader"),
	hdrShader("Shaders/hdr.vert.glsl", "Shaders/hdr.frag.glsl"),
	SSAOShader("Shaders/hdr.vert.glsl", "Shaders/ssao.frag.glsl"),
	GammaShader("Shaders/hdr.vert.glsl", "Shaders/gamma.frag.glsl"),
	ParticleShader("Shaders/instancedVert.shader", "Shaders/instancedFrag.shader"),
	TranslucentShader("Shaders/transparentVert.shader", "Shaders/transparentFrag.shader"),
	SkyboxShader("Shaders/skyboxVert.shader", "Shaders/skyboxFrag.shader")
{

	lightAccumulationShader.RegisterUniform("vec4", "uMaterial.Ambient");
	lightAccumulationShader.RegisterUniform("vec4", "uMaterial.Diffuse");
	lightAccumulationShader.RegisterUniform("vec4", "uMaterial.Specular");
	lightAccumulationShader.RegisterUniform("float", "uMaterial.Shininess");

	for (size_t i = 0; i < 8; ++i)
	{
		String baseName = String("uDirectionalLight[") + String::From(static_cast<int>(i)) + String("].");
		lightAccumulationShader.RegisterUniform("vec4", baseName + "ColorIntensity");
		lightAccumulationShader.RegisterUniform("vec4", baseName + "Direction");
	}
	lightAccumulationShader.RegisterUniform("int", "uDirectionalLightCount");

	lightAccumulationShader.RegisterUniform("int", "uLightCount");
	lightAccumulationShader.RegisterUniform("int", "uWorkGroupsX");
	lightAccumulationShader.RegisterUniform("int", "uWorkGroupsY");

	SkyboxShader.RegisterUniform("mat4", "uClipFromWorld");

	ParticleShader.RegisterUniform("float", "uTime");
	ParticleShader.RegisterUniform("mat4", "uScreenFromView");
	ParticleShader.RegisterUniform("mat4", "uViewFromWorld");
	ParticleShader.RegisterUniform("mat4", "uWorldFromModel");
	ParticleShader.RegisterUniform("vec4", "uColor");
}

void TiledForwardRenderer::Init()
{
	gConsole.LogInfo("TiledForwardRenderer::Init");

	CreateUtilityTextures();

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	// glEnable(GL_MULTISAMPLE);

	gConsole.LogInfo("TiledForwardRenderer::Init SCREEN_SIZE: ({},{})", SCREEN_SIZE_X, SCREEN_SIZE_Y);

	// So we need to create a depth map FBO
	// This will be used in the depth pass
	// Create a depth map frame buffer object and texture
	glGenFramebuffers(1, &FBOdepthMap);

	const ScreenSize screenSize = RDI->GetScreenSize();

	SCREEN_SIZE_X = screenSize.Width;
	SCREEN_SIZE_Y = screenSize.Height;

	glGenTextures(1, &preDepthBuffer);
	glBindTexture(GL_TEXTURE_2D, preDepthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCREEN_SIZE_X, SCREEN_SIZE_Y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, FBOdepthMap);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, preDepthBuffer, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Create a floating point HDR frame buffer and a floating point color buffer (as a texture)
	glGenFramebuffers(1, &FBOhdr);

	glGenTextures(1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_SIZE_X, SCREEN_SIZE_Y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &normalBuffer);
	glBindTexture(GL_TEXTURE_2D, normalBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_SIZE_X, SCREEN_SIZE_Y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// It will also need a depth component as a render buffer, attached to the hdrFBO
	GLuint rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_SIZE_X, SCREEN_SIZE_Y);

	glBindFramebuffer(GL_FRAMEBUFFER, FBOhdr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalBuffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, buffers);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Create pair of frame buffers for post process to swap
	glGenFramebuffers(1, &FBOpost0);

	glGenTextures(1, &postColorBuffer0);
	glBindTexture(GL_TEXTURE_2D, postColorBuffer0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_SIZE_X, SCREEN_SIZE_Y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, FBOpost0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postColorBuffer0, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &FBOpost1);

	glGenTextures(1, &postColorBuffer1);
	glBindTexture(GL_TEXTURE_2D, postColorBuffer1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_SIZE_X, SCREEN_SIZE_Y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, FBOpost1);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postColorBuffer1, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// Define work group sizes in x and y direction based off screen size and tile size (in pixels)
	workGroupsX = (SCREEN_SIZE_X + (SCREEN_SIZE_X % 16)) / 16;
	workGroupsY = (SCREEN_SIZE_Y + (SCREEN_SIZE_Y % 16)) / 16;
	size_t numberOfTiles = workGroupsX * workGroupsY;
	gConsole.LogInfo("TiledForwardRenderer::Init workGroups: ({},{}), numberOfTiles: {}", workGroupsX, workGroupsY, numberOfTiles);

	// Generate our shader storage buffers
	glGenBuffers(1, &lightBuffer);
	glGenBuffers(1, &visibleLightIndicesBuffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Light) * MAX_NUM_LIGHTS, 0, GL_DYNAMIC_DRAW);
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, visibleLightIndicesBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(VisibleIndex) * MAX_NUM_LIGHTS * numberOfTiles, 0, GL_STATIC_DRAW);

	SetupLightsBufferFromScene();

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void TiledForwardRenderer::Deinit()
{
	gConsole.LogInfo("TiledForwardRenderer::Deinit");
}

void TiledForwardRenderer::Render(World* world, const AARect& rect, const CameraComponent* cameraCmp)
{
	// gConsole.LogInfo("TiledForwardRenderer::Render");

	UpdateLightsBufferFromScene(world);

	RenderDepthPrePass(world, cameraCmp);

	ComputeLightCulling(world, cameraCmp);

	RenderOpaqueLit(world, cameraCmp);

	RenderSkybox(world, cameraCmp);

	RenderTranslucentUnlit(world, cameraCmp);

	RenderParticleUnlit(world, cameraCmp);

	PostTonemapper(world, rect, cameraCmp);

	// PostSSAO(cameraCmp);

	PostGamma();
}

void TiledForwardRenderer::RenderDepthPrePass(World* world, const CameraComponent* cameraCmp)
{
	depthShader.BindProgram();

	glBindFramebuffer(GL_FRAMEBUFFER, FBOdepthMap);
	glClear(GL_DEPTH_BUFFER_BIT);

	const Matrix& ScreenFromWorld = cameraCmp->GetClipFromWorld();
	for (auto componentsTuple : world->IterateComponents<MeshRenderingComponent>())
	{
		const MeshRenderingComponent* meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);
		if (meshCmp->IsTransparent()) continue;

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

void TiledForwardRenderer::DebugDepthPrepass(const CameraComponent* cameraCmp)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Weirdly, moving this call drops performance into the floor
	debugQuadDepthPrepassShader.BindProgram();
	debugQuadDepthPrepassShader.SetUniform("uNear", cameraCmp->GetClippingPlaneNear());
	debugQuadDepthPrepassShader.SetUniform("uFar", cameraCmp->GetClippingPlaneFar());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, preDepthBuffer);
	DrawQuad();
}

void TiledForwardRenderer::ComputeLightCulling(World* world, const CameraComponent* cameraCmp)
{
	float Time = (float)(world->GetWorldComponent<TimeWorldComponent>()->GetGameplayTime());

	// gConsole.LogInfo("TiledForwardRenderer::LightCulling Time: {}, workGroupsX: {}, workGroupsY: {}",
	// 	Time, workGroupsX, workGroupsY
	// );

	lightCullingShader.BindProgram();
	lightCullingShader.SetUniform("uViewFromWorld", cameraCmp->GetViewFromWorld());
	lightCullingShader.SetUniform("uClipFromWorld", cameraCmp->GetClipFromWorld());
	lightCullingShader.SetUniform("uClipFromView", cameraCmp->GetClipFromView());
	lightCullingShader.SetUniform("uScreenSizeX", (int)SCREEN_SIZE_X);
	lightCullingShader.SetUniform("uScreenSizeY", (int)SCREEN_SIZE_Y);
	lightCullingShader.SetUniform("uWorkGroupsX", (int)workGroupsX);
	lightCullingShader.SetUniform("uWorkGroupsY", (int)workGroupsY);
	lightCullingShader.SetUniform("uLightCount", (int)DynamicLighsInFrame);

	// Bind depth map texture to texture location 4 (which will not be used by any model texture)
	glActiveTexture(GL_TEXTURE4);
	glUniform1i(glGetUniformLocation(lightCullingShader.GetProgramHandle(), "uDepthMap"), 4);
	glBindTexture(GL_TEXTURE_2D, preDepthBuffer);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, visibleLightIndicesBuffer);

	glDispatchCompute(workGroupsX, workGroupsY, 1);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void TiledForwardRenderer::DebugLightAccum(World* world, const CameraComponent* cameraCmp)
{
	float Time = (float)(world->GetWorldComponent<TimeWorldComponent>()->GetGameplayTime());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	debugLightAccumShader.BindProgram();
	debugLightAccumShader.SetUniform("uTime", Time);
	debugLightAccumShader.SetUniform("uWorkGroupsX", (int)workGroupsX);
	debugLightAccumShader.SetUniform("uWorkGroupsY", (int)workGroupsY);
	debugLightAccumShader.SetUniform("uLightCount", (int)DynamicLighsInFrame);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, visibleLightIndicesBuffer);

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
}

void TiledForwardRenderer::RenderOpaqueLit(World* world, const CameraComponent* cameraCmp)
{
	// gConsole.LogInfo("TiledForwardRenderer::AccumulateLights");

	glBindFramebuffer(GL_FRAMEBUFFER, FBOhdr);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	const EntityTransform& cameraTransform = cameraCmp->GetTransform();

	lightAccumulationShader.BindProgram();
	lightAccumulationShader.SetUniform("uLightCount", (int)DynamicLighsInFrame);
	lightAccumulationShader.SetUniform("uWorkGroupsX", (int)workGroupsX);
	lightAccumulationShader.SetUniform("uWorkGroupsY", (int)workGroupsY);
	lightAccumulationShader.SetUniform("uViewPosition", cameraTransform.GetGlobalTranslation());

	glBindFragDataLocation(lightAccumulationShader.GetProgramHandle(), 0, "oColor");
	glBindFragDataLocation(lightAccumulationShader.GetProgramHandle(), 1, "oNormal");

	static const int MAX_LIGHT_COUNT_DIRECTIONAL = 8;
	int dirLightsCount = 0;
	for (const auto& componentsTuple : world->IterateComponents<DirectionalLightComponent>())
	{
		DirectionalLightComponent* dirLightCmp = std::get<DirectionalLightComponent*>(componentsTuple);
		const EntityTransform& transform = dirLightCmp->GetTransform();
		String baseName = String("uDirectionalLight[") + String::From(dirLightsCount) + String("].");
		Color ColorIntensity = dirLightCmp->GetColor();
		ColorIntensity.A = dirLightCmp->GetIntensity();
		lightAccumulationShader.SetUniform(baseName + "ColorIntensity", ColorIntensity);
		lightAccumulationShader.SetUniform(baseName + "Direction", MovementSystem::GetGlobalForward(transform));

		++dirLightsCount;
		if (dirLightsCount == MAX_LIGHT_COUNT_DIRECTIONAL)
			break;
	}
	lightAccumulationShader.SetUniform("uDirectionalLightCount", dirLightsCount);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, visibleLightIndicesBuffer);

	const Matrix& ClipFromWorld = cameraCmp->GetClipFromWorld();
	for (auto componentsTuple : world->IterateComponents<MeshRenderingComponent>())
	{
		const MeshRenderingComponent* meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);
		if (meshCmp->IsTransparent()) continue;

		const EntityTransform& transform = meshCmp->GetTransform();
		const Matrix& WorldFromModel = transform.GetWorldFromModel();
		lightAccumulationShader.SetUniform("uClipFromModel", ClipFromWorld * WorldFromModel);
		lightAccumulationShader.SetUniform("uWorldFromModel", WorldFromModel);
		
		int i = 0;
		for (const MeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
		{
			PhongMaterial material = meshCmp->GetMaterial(i);
			lightAccumulationShader.SetUniform("uMaterial.Ambient", material.AmbientColor);
			lightAccumulationShader.SetUniform("uMaterial.Diffuse", material.DiffuseColor);
			lightAccumulationShader.SetUniform("uMaterial.Specular", material.SpecularColor);
			lightAccumulationShader.SetUniform("uMaterial.Shininess", material.Shininess);

			const GLMeshDeviceProxy* meshProxy = static_cast<const GLMeshDeviceProxy*>(subMesh->GetMeshProxy());
			glBindVertexArray(meshProxy->GetVAO());

			const TextureResource* DiffuseTexture = subMesh->GetMeshData().GetDiffTexture();
			GLuint DiffuseID = DiffuseTexture == nullptr
				? FallbackWhiteTexture
				: static_cast<const GLTextureDeviceProxy*>(DiffuseTexture->GetTextureProxy())->GetTextureID();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, DiffuseID);
			lightAccumulationShader.SetUniform("uAlbedoMap", 0);

			const TextureResource* NormalMap = subMesh->GetMeshData().GetNormalMap();
			GLuint NormalMapID = NormalMap == nullptr
				? FallbackNormalMap
				: static_cast<const GLTextureDeviceProxy*>(NormalMap->GetTextureProxy())->GetTextureID();
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, NormalMapID);
			lightAccumulationShader.SetUniform("uNormalMap", 1);

			const TextureResource* SpecularTexture = subMesh->GetMeshData().GetSpecularMap();
			GLuint SpecularID = SpecularTexture == nullptr
				? FallbackWhiteTexture
				: static_cast<const GLTextureDeviceProxy*>(SpecularTexture->GetTextureProxy())->GetTextureID();
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, SpecularID);
			lightAccumulationShader.SetUniform("uSpecularMap", 2);

			glDrawElements(GL_TRIANGLES, (GLsizei)subMesh->GetMeshData().GetTriangleCount() * 3, GL_UNSIGNED_INT, NULL);
			++i;
		}
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindVertexArray(0);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TiledForwardRenderer::RenderSkybox(World* world, const CameraComponent* cameraCmp)
{
	const SkyboxWorldComponent* SkyboxWorldCmp = world->GetWorldComponent<SkyboxWorldComponent>();
	if (SkyboxWorldCmp != nullptr)
	{
		const Matrix ClipFromView = cameraCmp->GetClipFromView();
		Matrix ViewFromWorld = cameraCmp->GetViewFromWorld();
		// center cube in view space by setting translation to 0 for x, y and z.
		// SetTranslation resets Matrix to identity
		ViewFromWorld.Data[3] = 0.0f;
		ViewFromWorld.Data[7] = 0.0f;
		ViewFromWorld.Data[11] = 0.0f;

		Matrix ClipFromWorld = ClipFromView * ViewFromWorld;

		SkyboxShader.BindProgram();
		SkyboxShader.SetUniform("uClipFromWorld", ClipFromWorld);

		GLuint CubemapID = static_cast<const GLCubemapDeviceProxy*>(SkyboxWorldCmp->GetCubemap().GetTextureProxy())->GetTextureID();

		glBindFramebuffer(GL_FRAMEBUFFER, FBOhdr);
		
		glBindFragDataLocation(lightAccumulationShader.GetProgramHandle(), 0, "color");
		glBindFragDataLocation(lightAccumulationShader.GetProgramHandle(), 1, "normal");
		
		glDisable(GL_CULL_FACE);
		glDepthFunc(GL_LEQUAL);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, CubemapID);

		glBindVertexArray(RDI->PrimitiveRenderingCube->VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		glEnable(GL_CULL_FACE);
		glDepthFunc(GL_LESS);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void TiledForwardRenderer::RenderTranslucentUnlit(World* world, const CameraComponent* cameraCmp)
{
	gConsole.LogInfo("TiledForwardRenderer::RenderTranslucentUnlit");
	
	glBindFramebuffer(GL_FRAMEBUFFER, FBOhdr);

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	const EntityTransform& cameraTransform = cameraCmp->GetTransform();
	TranslucentShader.BindProgram();
	TranslucentShader.SetUniform("uViewPosition", cameraTransform.GetGlobalTranslation());

	const Matrix& ClipFromWorld = cameraCmp->GetClipFromWorld();
	for (auto componentsTuple : world->IterateComponents<MeshRenderingComponent>())
	{
		const MeshRenderingComponent* meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);
		if (!meshCmp->IsTransparent()) continue;

		const EntityTransform& transform = meshCmp->GetTransform();
		const Matrix& WorldFromModel = transform.GetWorldFromModel();
		TranslucentShader.SetUniform("uClipFromModel", ClipFromWorld * WorldFromModel);
		TranslucentShader.SetUniform("uWorldFromModel", WorldFromModel);

		int i = 0;
		for (const MeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
		{
			PhongMaterial material = meshCmp->GetMaterial(i);
			TranslucentShader.SetUniform("uMaterial.Ambient", material.AmbientColor);
			TranslucentShader.SetUniform("uMaterial.Diffuse", material.DiffuseColor);
			TranslucentShader.SetUniform("uMaterial.Specular", material.SpecularColor);
			TranslucentShader.SetUniform("uMaterial.Shininess", material.Shininess);

			const GLMeshDeviceProxy* meshProxy = static_cast<const GLMeshDeviceProxy*>(subMesh->GetMeshProxy());
			glBindVertexArray(meshProxy->GetVAO());

			const TextureResource* DiffuseTexture = subMesh->GetMeshData().GetDiffTexture();
			GLuint DiffuseID = DiffuseTexture == nullptr
				? FallbackWhiteTexture
				: static_cast<const GLTextureDeviceProxy*>(DiffuseTexture->GetTextureProxy())->GetTextureID();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, DiffuseID);
			TranslucentShader.SetUniform("uAlbedoMap", 0);

			const TextureResource* NormalMap = subMesh->GetMeshData().GetNormalMap();
			GLuint NormalMapID = NormalMap == nullptr
				? FallbackNormalMap
				: static_cast<const GLTextureDeviceProxy*>(NormalMap->GetTextureProxy())->GetTextureID();
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, NormalMapID);
			TranslucentShader.SetUniform("uNormalMap", 1);

			const TextureResource* SpecularTexture = subMesh->GetMeshData().GetSpecularMap();
			GLuint SpecularID = SpecularTexture == nullptr
				? FallbackWhiteTexture
				: static_cast<const GLTextureDeviceProxy*>(SpecularTexture->GetTextureProxy())->GetTextureID();
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, SpecularID);
			TranslucentShader.SetUniform("uSpecularMap", 2);

			glDrawElements(GL_TRIANGLES, (GLsizei)subMesh->GetMeshData().GetTriangleCount() * 3, GL_UNSIGNED_INT, NULL);
			++i;
		}
		
	}
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindVertexArray(0);

	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TiledForwardRenderer::RenderParticleUnlit(World* world, const CameraComponent* cameraCmp)
{
	// gConsole.LogInfo("TiledForwardRenderer::RenderParticleUnlit");

	float Time = (float)TimeSystem::GetTimerElapsedTime(world, eEngineTimer::GAMEPLAY);
	const Matrix& ViewFromWorld = cameraCmp->GetViewFromWorld();
	const Matrix& ScreenFromView = cameraCmp->GetClipFromView();

	glBindFramebuffer(GL_FRAMEBUFFER, FBOhdr);

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	// TODO test these blending options
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_ONE, GL_ONE);
	//glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);

	ParticleShader.BindProgram();
	ParticleShader.SetUniform("uTime", Time);
	ParticleShader.SetUniform("uScreenFromView", ScreenFromView);

	for (auto componentsTuple : world->IterateComponents<ParticleComponent>())
	{
		const ParticleComponent* particleCmp = std::get<ParticleComponent*>(componentsTuple);
		const EntityTransform& transform = particleCmp->GetTransform();
		const Matrix& WorldFromModel = particleCmp->GetEmitter()->GetSettings().SimulationSpace == ParticleEmitter::eSimulationSpace::LOCAL_SPACE
			? transform.GetWorldFromModel()
			: Matrix();

		ParticleShader.SetUniform("uViewFromWorld", ViewFromWorld);
		ParticleShader.SetUniform("uWorldFromModel", WorldFromModel);

		ParticleEmitter::Settings emitterSettings = particleCmp->GetEmitter()->GetSettings();
		ParticleShader.SetUniform("uEmitterColor", emitterSettings.BaseColor);

		SpritesheetSettings spriteSettings = emitterSettings.SpritesheetSettings;
		ParticleShader.SetUniform("uSpriteColor", spriteSettings.SpriteColor);
		float startFrame = spriteSettings.IsRandomStartFrame ? RandomRange(0.0f, spriteSettings.SubImages.X * spriteSettings.SubImages.Y) : spriteSettings.StartFrame;
		ParticleShader.SetUniform("uSpriteStartFrame", startFrame);
		ParticleShader.SetUniform("uSpriteSpeed", spriteSettings.Speed);
		ParticleShader.SetUniform("uSpriteSubImages", spriteSettings.SubImages.X, spriteSettings.SubImages.Y);

		GLsizei partileLen = (GLsizei)(particleCmp->GetEmitter()->GetInstancesCount());
		const TextureResource* Texture = particleCmp->GetEmitter()->GetSpritesheet();
		const GLParticleDeviceProxy* particleProxy = static_cast<const GLParticleDeviceProxy*>(particleCmp->GetEmitter()->GetParticleProxy());
		GLuint particleVAO = particleProxy->GetVAO();

		GLuint TextureID = Texture == nullptr
			? FallbackWhiteTexture
			: static_cast<const GLTextureDeviceProxy*>(Texture->GetTextureProxy())->GetTextureID();

		ParticleShader.SetUniform("uHasSprite", Texture == nullptr ? 0.0f : 1.0f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureID);

		glBindVertexArray(particleVAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, partileLen);
		glBindVertexArray(0);
		CHECK_GL_ERR();

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TiledForwardRenderer::PostTonemapper(World* world, const AARect& rect, const CameraComponent* cameraCmp)
{
	// gConsole.LogInfo("TiledForwardRenderer::Tonemapper");

	float exposure = 1.0f;
	const PostprocessSettingsComponent* postCmp = cameraCmp->GetSibling<PostprocessSettingsComponent>();
	if (postCmp) {
		exposure = postCmp->Exposure;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Weirdly, moving this call drops performance into the floor
	
	glBindFramebuffer(GL_FRAMEBUFFER, FBOpost0);

	hdrShader.BindProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	hdrShader.SetUniform("uExposure", exposure);
	DrawQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TiledForwardRenderer::PostGamma()
{
	GammaShader.BindProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, postColorBuffer0);
	GammaShader.SetUniform("uGamma", 2.2f);
	DrawQuad();
}

void TiledForwardRenderer::PostSSAO(const CameraComponent* cameraCmp)
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBOpost1);

	// based on: http://john-chapman-graphics.blogspot.com/2013/01/ssao-tutorial.html
	static Dynarray<Vector> SSAOKernel;
	if (SSAOKernel.GetSize() <= 0) {
		gConsole.LogInfo("TiledForwardRenderer::PostSSAO generating kernel");
		for (int i = 0; i < 16; ++i) {
			SSAOKernel.PushBack(Vector(
				RandomRange(-1.0f, 1.0f),
				RandomRange(-1.0f, 1.0f),
				RandomRange( 0.0f, 1.0f)
			));
			SSAOKernel[i].Normalize();
			SSAOKernel[i] *= RandomRange(0.0f, 1.0f);
			float scale = float(i) / 16.0;
			scale = Lerp(0.1f, 1.0f, scale * scale);
			SSAOKernel[i] *= scale;
			gConsole.LogInfo("TiledForwardRenderer::PostSSAO kernel #{} {}", i, SSAOKernel[i]);
		}
	}

	SSAOShader.BindProgram();
	for (int i = 0; i < 16; ++i) {
		SSAOShader.SetUniform(String("uKernel[") + String::From(i) + "]", SSAOKernel[i]);
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, postColorBuffer0);
	SSAOShader.SetUniform("uBeauty", 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalBuffer);
	SSAOShader.SetUniform("uNormal", 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, preDepthBuffer);
	SSAOShader.SetUniform("uDepth", 2);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, SSAONoiseMap);
	SSAOShader.SetUniform("uNoise", 3);
	SSAOShader.SetUniform("uScreenRes", Vector(
		RDI->GetScreenSize().Width,
		RDI->GetScreenSize().Height,
		1.0f / ((float)RDI->GetScreenSize().Width),
		1.0f / ((float)RDI->GetScreenSize().Height))
	);
	SSAOShader.SetUniform("uFarClippingPlane", cameraCmp->GetClippingPlaneFar());
	SSAOShader.SetUniform("uBias", 0.3f);
	SSAOShader.SetUniform("uViewFromWorld", cameraCmp->GetViewFromWorld());
	DrawQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TiledForwardRenderer::SetupLightsBufferFromScene()
{
	if (lightBuffer == 0)
	{
		return;
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightBuffer);
	Light *lights = (Light*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

	for (int i = 0; i < MAX_NUM_LIGHTS; ++i)
	{
		Light &light = lights[i];
		light.Position = Vector::ZERO;
		light.Color = Vector::ZERO;
		light.RangeIntensity = Vector(-1.0f, 0.0f, 0.0f);
	}

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void TiledForwardRenderer::UpdateLightsBufferFromScene(World* world)
{
	// gConsole.LogInfo("TiledForwardRenderer::UpdateLightsBufferFromScene");

	Dynarray<Vector> Positions;
	Dynarray<Vector> Color;
	Dynarray<Vector> RangeIntensity;

	DynamicLighsInFrame = 0;
	for (const auto& componentsTuple : world->IterateComponents<PointLightComponent>())
	{
		PointLightComponent* pointLightCmp = std::get<PointLightComponent*>(componentsTuple);
		const EntityTransform& transform = pointLightCmp->GetTransform();

		Positions.PushBack(transform.GetGlobalTranslation());
		Color.PushBack(Vector(pointLightCmp->GetColor()));
		RangeIntensity.PushBack(Vector(pointLightCmp->GetRange(), pointLightCmp->GetIntensity(), 0.0f));

		//		gConsole.LogInfo("TiledForwardRenderer::UpdateLightsBufferFromScene get Position: {}, Radius: {}",
		//			transform.GetGlobalTranslation(), pointLightCmp->GetRange());

		++DynamicLighsInFrame;

		if (DynamicLighsInFrame > MAX_NUM_LIGHTS)
		{
			gConsole.LogInfo("TiledForwardRenderer::UpdateLightsBufferFromScene more lights than supported by renderer({})", MAX_NUM_LIGHTS);
			break;
		}
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightBuffer);
	Light *lights = (Light*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

	for (int i = 0; i < DynamicLighsInFrame; ++i)
	{
		Light &light = lights[i];
		light.Position = Positions[i];
		light.Color = Color[i];
		light.RangeIntensity = RangeIntensity[i];

		// 		gConsole.LogInfo("TiledForwardRenderer::UpdateLightsBufferFromScene set Position: {}, Radius: {}",
		// 			light.Position, light.Radius);
	}

	int remainingSlots = MAX_NUM_LIGHTS - DynamicLighsInFrame;
	for (int i = 0; i < remainingSlots; ++i)
	{
		Light &light = lights[DynamicLighsInFrame + i];
		light.Position = Vector::ZERO;
		light.Color = Vector::ZERO;
		light.RangeIntensity = Vector(-1.0f, 0.0f, 0.0f);
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

void TiledForwardRenderer::CreateUtilityTextures()
{
	gConsole.LogInfo("TiledForwardRenderer::CreateUtilityTextures");

	GLubyte data[] = { 255, 255, 255, 255 };
	glGenTextures(1, &FallbackWhiteTexture);
	glBindTexture(GL_TEXTURE_2D, FallbackWhiteTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	CHECK_GL_ERR();

	GLubyte dataDefaultNormal[] = { 128, 128, 255 };
	glGenTextures(1, &FallbackNormalMap);
	glBindTexture(GL_TEXTURE_2D, FallbackNormalMap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, dataDefaultNormal);
	CHECK_GL_ERR();

	Vector noise[16];
	for (int i = 0; i < 16; ++i) {
		noise[i] = Vector(
			RandomRange(-1.0f, 1.0f),
			RandomRange(-1.0f, 1.0f),
			0.0f
		);
		noise[i].Normalize();
	}

	GLubyte dataSSAONoise[16 * 3];
	for (int i = 0; i < 16; i += 3) {
		Vector positive = noise[i] * 0.5f + 0.5f;
		dataSSAONoise[i + 0] = (GLbyte)Clamp((int)(positive.X * 255.0f), 0, 255);
		dataSSAONoise[i + 1] = (GLbyte)Clamp((int)(positive.Y * 255.0f), 0, 255);
		dataSSAONoise[i + 2] = (GLbyte)Clamp((int)(positive.Z * 255.0f), 0, 255);
	}

	glGenTextures(1, &SSAONoiseMap);
	glBindTexture(GL_TEXTURE_2D, SSAONoiseMap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 4, 4, 0, GL_RGB, GL_UNSIGNED_BYTE, dataSSAONoise);
	CHECK_GL_ERR();
}