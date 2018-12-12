#include <PolyRenderingDeviceGLPCH.hpp>

#include <Pipeline/ShadowMapPass.hpp>
#include <GLRenderingDevice.hpp>
#include <IRendererInterface.hpp>

using namespace Poly;

Matrix Poly::GetProjectionForShadowMap(const SceneView& sceneView, int shadowmapSize)
{
	ASSERTE(sceneView.DirectionalLights.GetSize() > 0, "GetProjectionForShadowMap has no directional light in scene view");
	const DirectionalLightComponent* dirLightCmp = sceneView.DirectionalLights[0];

	Vector lightForward = dirLightCmp->GetTransform().GetGlobalForward();
	Vector lightUp = dirLightCmp->GetTransform().GetGlobalUp();
	Matrix lightViewFromWorld = Matrix(Vector::ZERO, lightForward, lightUp);

	Vector shadowAABBExtentsInLS = sceneView.DirShadowAABBInLS.GetSize() * 0.5f;
	Matrix clipFromLightView;
	// n > f, because we are looking down the negative z - axis at this volume of space
	clipFromLightView.SetOrthographicZO(
		-shadowAABBExtentsInLS.Y,	// bottom
		shadowAABBExtentsInLS.Y,	// top
		-shadowAABBExtentsInLS.X,	// left
		shadowAABBExtentsInLS.X,	// right
		0.0f,						// near
		-shadowAABBExtentsInLS.Z	// far
	);

	Matrix lightViewFromModel;
	Vector shadowAABBCenterInLS = sceneView.DirShadowAABBInLS.GetCenter();
	lightViewFromModel.SetTranslation(-shadowAABBCenterInLS);

	Matrix clipFromWorld = clipFromLightView * lightViewFromModel * lightViewFromWorld;
	StablizeShadowProjection(clipFromWorld, shadowmapSize);

	return clipFromWorld;
}

void Poly::StablizeShadowProjection(Poly::Matrix& clipFromWorld, int shadowmapSize)
{
	// based on https://mynameismjp.wordpress.com/2013/09/10/shadow-maps/
	// Stabilize shadow map: move in texel size increments
	// round matrix translation to texel size increments
	Vector shadowOrigin = clipFromWorld * Vector::ZERO;
	shadowOrigin *= 0.5f * shadowmapSize;

	Vector roundedOrigin = Vector(
		roundf(shadowOrigin.X),
		roundf(shadowOrigin.Y),
		0.0f
	);

	Vector roundOffset = roundedOrigin - shadowOrigin;
	roundOffset *= 2.0f / shadowmapSize;
	roundOffset.Z = 0.0f;
	roundOffset.W = 0.0f;

	clipFromWorld.Data[3] += roundOffset.X;
	clipFromWorld.Data[7] += roundOffset.Y;
}

int Poly::GetShadowMapSize(const eShadowMapSize shadowMapSize)
{
	switch (shadowMapSize)
	{
		case eShadowMapSize::SIZE_512:	return 512;
		case eShadowMapSize::SIZE_1024:	return 1024;
		case eShadowMapSize::SIZE_2048:	return 2048;
		case eShadowMapSize::SIZE_4096:	return 4096;
		default:
			ASSERTE(false, "Uknown shadowmap size setting enum");
			return 512;
	}
}

ShadowMapPass::ShadowMapPass(GLRenderingDevice* rdi)
	: RDI(rdi),
	ShadowMapShader("Shaders/shadowMap.vert.glsl", "Shaders/shadowMap.frag.glsl"),
	EVSMResolveShader("Shaders/hdr.vert.glsl", "Shaders/evsm.resolve.frag.glsl"),
	EVSMBlurShader("Shaders/hdr.vert.glsl", "Shaders/evsm.blur.frag.glsl")
{
	ShadowMapShader.RegisterUniform("mat4", "uClipFromModel");

	EVSMResolveShader.RegisterUniform("sampler2D", "uDepth");
	EVSMResolveShader.RegisterUniform("float", "uNear");
	EVSMResolveShader.RegisterUniform("float", "uFar");
	EVSMResolveShader.RegisterUniform("float", "uPositiveExponent");
	EVSMResolveShader.RegisterUniform("float", "uNegativeExponent");

	EVSMBlurShader.RegisterUniform("sampler2D", "uEVSMap");
	EVSMBlurShader.RegisterUniform("int", "uIsHorizontal");
}

ShadowMapPass::~ShadowMapPass()
{
}

void ShadowMapPass::Init(const SceneView& sceneView)
{
	gConsole.LogInfo("ShadowMapPass::Init");

	if (sceneView.SettingsCmp == nullptr)
	{
		ShadowMapResolution = 1; // create dummy resource
		ShadowType = eShadowType::NONE;
	}
	else
	{
		ShadowMapResolution = GetShadowMapSize(sceneView.SettingsCmp->ShadowMapSize);
		ShadowType = sceneView.SettingsCmp->ShadowType;
	}

	float shadowBorderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glGenTextures(1, &DirShadowMapDepth);
	glBindTexture(GL_TEXTURE_2D, DirShadowMapDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, ShadowMapResolution, ShadowMapResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, shadowBorderColor);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &DirShadowMapColor);
	glBindTexture(GL_TEXTURE_2D, DirShadowMapColor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, ShadowMapResolution, ShadowMapResolution, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, shadowBorderColor);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &FBOShadowDepthMap);
	glBindFramebuffer(GL_FRAMEBUFFER, FBOShadowDepthMap);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DirShadowMapDepth, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, DirShadowMapColor, 0);
	// glDrawBuffer(GL_NONE);
	// glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// CHECK_GL_ERR();
	CHECK_FBO_STATUS();

	// Create pair of frame buffers for evsm
	glGenTextures(1, &EVSMap0);
	glBindTexture(GL_TEXTURE_2D, EVSMap0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, ShadowMapResolution / 2, ShadowMapResolution / 2, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &FBOShadowMapResolve0);
	glBindFramebuffer(GL_FRAMEBUFFER, FBOShadowMapResolve0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, EVSMap0, 0);

	glGenTextures(1, &EVSMap1);
	glBindTexture(GL_TEXTURE_2D, EVSMap1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, ShadowMapResolution / 2, ShadowMapResolution / 2, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &FBOShadowMapResolve1);
	glBindFramebuffer(GL_FRAMEBUFFER, FBOShadowMapResolve1);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, EVSMap1, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	CHECK_FBO_STATUS();
}

void ShadowMapPass::Deinit()
{
	gConsole.LogInfo("ShadowMapPass::Deinit");

	if (FBOShadowDepthMap > 0)
		glDeleteFramebuffers(1, &FBOShadowDepthMap);

	if (EVSMap0 > 0)
		glDeleteTextures(1, &EVSMap0);

	if (FBOShadowMapResolve0 > 0)
		glDeleteFramebuffers(1, &FBOShadowMapResolve0);

	if (FBOShadowMapResolve1 > 0)
		glDeleteFramebuffers(1, &FBOShadowMapResolve1);
}

void ShadowMapPass::Render(const SceneView& sceneView)
{	
	// gConsole.LogInfo("ShadowMapPass::Render");

	if (sceneView.DirectionalLights.GetSize() < 1)
		return;

	if (sceneView.SettingsCmp == nullptr
		|| sceneView.SettingsCmp->ShadowType == eShadowType::NONE)
		return;

	switch (sceneView.SettingsCmp->ShadowType)
	{
		case eShadowType::PCF:
			RenderPCF(sceneView); 
			break;
		case eShadowType::EVSM2:
		case eShadowType::EVSM4:
			RenderEVSM(sceneView);
			break;
		default:
			break;
	}
}

void ShadowMapPass::RenderPCF(const SceneView& sceneView)
{
	glViewport(0, 0, ShadowMapResolution, ShadowMapResolution);
	glBindFramebuffer(GL_FRAMEBUFFER, FBOShadowDepthMap);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glDepthMask(GL_TRUE);

	Matrix orthoDirLightFromWorld = GetProjectionForShadowMap(sceneView, ShadowMapResolution);

	ShadowMapShader.BindProgram();

	for (const MeshRenderingComponent* meshCmp : sceneView.DirShadowOpaqueQueue)
	{
		const Matrix& worldFromModel = meshCmp->GetTransform().GetWorldFromModel();
		ShadowMapShader.SetUniform("uClipFromModel", orthoDirLightFromWorld * worldFromModel);

		for (const MeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
		{
			glBindVertexArray(subMesh->GetMeshProxy()->GetResourceID());
			glDrawElements(GL_TRIANGLES, (GLsizei)subMesh->GetMeshData().GetTriangleCount() * 3, GL_UNSIGNED_INT, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindVertexArray(0);
		}
	}

	glCullFace(GL_BACK);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMapPass::RenderEVSM(const SceneView& sceneView)
{
	glViewport(0, 0, ShadowMapResolution, ShadowMapResolution);
	glBindFramebuffer(GL_FRAMEBUFFER, FBOShadowDepthMap);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glDepthMask(GL_TRUE);

	Matrix orthoDirLightFromWorld = GetProjectionForShadowMap(sceneView, ShadowMapResolution);

	ShadowMapShader.BindProgram();

	for (const MeshRenderingComponent* meshCmp : sceneView.DirShadowOpaqueQueue)
	{
		const Matrix& worldFromModel = meshCmp->GetTransform().GetWorldFromModel();
		ShadowMapShader.SetUniform("uClipFromModel", orthoDirLightFromWorld * worldFromModel);

		for (const MeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
		{
			glBindVertexArray(subMesh->GetMeshProxy()->GetResourceID());
			glDrawElements(GL_TRIANGLES, (GLsizei)subMesh->GetMeshData().GetTriangleCount() * 3, GL_UNSIGNED_INT, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindVertexArray(0);
		}
	}

	glViewport(0, 0, ShadowMapResolution / 2, ShadowMapResolution / 2);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, FBOShadowMapResolve0);

	EVSMResolveShader.BindProgram();
	EVSMResolveShader.BindSampler("uDepth", 0, DirShadowMapColor);
	EVSMResolveShader.SetUniform("uNear", sceneView.CameraCmp->GetClippingPlaneNear());
	EVSMResolveShader.SetUniform("uFar", sceneView.CameraCmp->GetClippingPlaneFar());
	EVSMResolveShader.SetUniform("uPositiveExponent", sceneView.SettingsCmp->EVSMPositiveExponent);
	EVSMResolveShader.SetUniform("uNegativeExponent", sceneView.SettingsCmp->EVSMNegativeExponent);

	glBindVertexArray(RDI->PrimitivesQuad->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, FBOShadowMapResolve1);
	EVSMBlurShader.BindProgram();
	EVSMBlurShader.BindSampler("uEVSMap", 0, EVSMap0);
	EVSMBlurShader.SetUniform("uIsHorizontal", 1);

	glBindVertexArray(RDI->PrimitivesQuad->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, FBOShadowMapResolve0);
	EVSMBlurShader.BindProgram();
	EVSMBlurShader.BindSampler("uEVSMap", 0, EVSMap1);
	EVSMBlurShader.SetUniform("uIsHorizontal", 0);

	glBindVertexArray(RDI->PrimitivesQuad->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}