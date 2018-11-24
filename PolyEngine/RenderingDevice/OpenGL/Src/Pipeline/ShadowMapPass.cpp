#include <PolyRenderingDeviceGLPCH.hpp>

#include <Pipeline/ShadowMapPass.hpp>
#include <GLRenderingDevice.hpp>

using namespace Poly;

Matrix Poly::GetProjectionForShadowMap(const DirectionalLightComponent* dirLightCmp, int shadowmapSize)
{
	// ASSERTE(sceneView.DirectionalLights.GetSize() > 0, "GetProjectionForShadowMap has no directional light in scene view");
	// const DirectionalLightComponent* dirLightCmp = sceneView.DirectionalLights[0];

	Vector lightForward = dirLightCmp->GetTransform().GetGlobalForward();
	Vector lightUp = dirLightCmp->GetTransform().GetGlobalUp();
	Matrix lightViewFromWorld = Matrix(Vector::ZERO, lightForward, lightUp);

	Vector shadowAABBExtentsInLS = dirLightCmp->ShadowAABBInLS.GetSize() * 0.5f;
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
	Vector shadowAABBCenterInLS = dirLightCmp->ShadowAABBInLS.GetCenter();
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
	// float shadowmapSize = (float)SHADOWMAP_SIZE;
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

void Poly::ShadowMapPass::Init()
{
	float shadowBorderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glGenTextures(1, &DirShadowMapDepth);
	glBindTexture(GL_TEXTURE_2D, DirShadowMapDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWMAP_SIZE, SHADOWMAP_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, shadowBorderColor);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &DirShadowMapColor);
	glBindTexture(GL_TEXTURE_2D, DirShadowMapColor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SHADOWMAP_SIZE, SHADOWMAP_SIZE, 0, GL_RGB, GL_FLOAT, NULL);
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, SHADOWMAP_SIZE / 2, SHADOWMAP_SIZE / 2, 0, GL_RGB, GL_FLOAT, NULL);
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, SHADOWMAP_SIZE / 2, SHADOWMAP_SIZE / 2, 0, GL_RGB, GL_FLOAT, NULL);
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

void Poly::ShadowMapPass::Deinit()
{
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
	gConsole.LogInfo("ShadowMapPass::Render");

	if (sceneView.DirectionalLights.GetSize() < 1)
		return;

	glViewport(0, 0, SHADOWMAP_SIZE, SHADOWMAP_SIZE);
	glBindFramebuffer(GL_FRAMEBUFFER, FBOShadowDepthMap);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glDepthMask(GL_TRUE);
	// glEnable(GL_POLYGON_OFFSET_FILL);
	// glPolygonOffset(sceneView.CameraCmp->PolygonOffset, sceneView.CameraCmp->PolygonUnits);

	Matrix orthoDirLightFromWorld = GetProjectionForShadowMap(sceneView.DirectionalLights[0], SHADOWMAP_SIZE);

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

	// glDisable(GL_POLYGON_OFFSET_FILL);
	// glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, SHADOWMAP_SIZE / 2, SHADOWMAP_SIZE / 2);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, FBOShadowMapResolve0);

	EVSMResolveShader.BindProgram();
	EVSMResolveShader.BindSampler("uDepth", 0, DirShadowMapColor);
	EVSMResolveShader.SetUniform("uNear", sceneView.CameraCmp->GetClippingPlaneNear());
	EVSMResolveShader.SetUniform("uFar", sceneView.CameraCmp->GetClippingPlaneFar());
	EVSMResolveShader.SetUniform("uPositiveExponent", sceneView.CameraCmp->EVSMExponentPositive);
	EVSMResolveShader.SetUniform("uNegativeExponent", sceneView.CameraCmp->EVSMExponentNegative);

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
