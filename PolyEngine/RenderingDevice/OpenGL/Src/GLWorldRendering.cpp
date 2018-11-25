#include <PolyRenderingDeviceGLPCH.hpp>

#include <GLRenderingDevice.hpp>

#include <algorithm>    // std::min

#include "Proxy/GLTextFieldBufferDeviceProxy.hpp"
#include "Proxy/GLTextureDeviceProxy.hpp"
#include "Pipeline/RenderingPassBase.hpp"
#include "Proxy/GLMeshDeviceProxy.hpp"
#include "Common/GLUtils.hpp"
#include "Common/PrimitiveCube.hpp"
#include "Common/PrimitiveQuad.hpp"
#include "Debugging//DebugDrawSystem.hpp"
#include "ForwardRenderer.hpp"
#include "TiledForwardRenderer.hpp"

using namespace Poly;

void GLRenderingDevice::Init()
{
	gConsole.LogInfo("GLRenderingDevice::Init");

	GetExtensions();
	
	PrimitivesQuad = std::make_unique<PrimitiveQuad>();
	PrimitivesCube = std::make_unique<PrimitiveCube>();

	CreateUtilityTextures();

	Renderer = CreateRenderer();
	Renderer->Init();
}

IRendererInterface* GLRenderingDevice::CreateRenderer()
{
	IRendererInterface* renderer = nullptr;

	switch (RendererType)
	{
		case GLRenderingDevice::eRendererType::FORWARD:
			renderer = new ForwardRenderer(this);
			break;

		case GLRenderingDevice::eRendererType::TILED_FORWARD:
			renderer = new TiledForwardRenderer(this);
			break;

		default:
			ASSERTE(false, "Uknown eRenderingModeType");
	}

	return renderer;
}

void GLRenderingDevice::RenderWorld(Scene* world)
{
	// For each visible viewport draw it
	for (auto& kv : world->GetWorldComponent<ViewportWorldComponent>()->GetViewports())
	{
		SceneView sceneView(world, kv.second);
		
		FillSceneView(sceneView);

		Renderer->Render(sceneView);
	}

	// Signal frame end
	EndFrame();
}

void GLRenderingDevice::FillSceneView(SceneView& sceneView)
{
	for (const auto& [dirLightCmp] : sceneView.SceneData->IterateComponents<DirectionalLightComponent>())
	{
		sceneView.DirectionalLights.PushBack(dirLightCmp);
	}

	for (const auto& [pointLightCmp] : sceneView.SceneData->IterateComponents<PointLightComponent>())
	{
		sceneView.PointLights.PushBack(pointLightCmp);
	}

	Dynarray<MeshRenderingComponent*> meshCmps;
	for (const auto& [meshCmp] : sceneView.SceneData->IterateComponents<MeshRenderingComponent>())
	{
		meshCmps.PushBack(meshCmp);
	}

	for (auto& meshCmp : meshCmps)
	{
		if (sceneView.CameraCmp->IsVisibleToCamera(meshCmp->GetOwner()))
		{
			if (meshCmp->GetBlendingMode() == eBlendingMode::OPAUQE)
			{
				sceneView.OpaqueQueue.PushBack(meshCmp);
			}
			else if (meshCmp->GetBlendingMode() == eBlendingMode::TRANSLUCENT)
			{
				sceneView.TranslucentQueue.PushBack(meshCmp);
			}
		}
	}

	FillDirLightQueue(sceneView, meshCmps);

	for (auto& meshCmp : meshCmps)
	{
		if (meshCmp->IsShadowCaster)
			sceneView.DirShadowOpaqueQueue.PushBack(meshCmp);
	}
}

void DrawFrustumPoints(Scene* scene, Dynarray<Vector> &cornersInWorld, Color color)
{
	for (size_t i = 0; i < 4; ++i)
		DebugDrawSystem::DrawLine(scene, cornersInWorld[i], cornersInWorld[i + 4], color);

	for (size_t i = 0; i < 2; ++i)
	{
		DebugDrawSystem::DrawLine(scene, cornersInWorld[0 + i * 4], cornersInWorld[1 + i * 4], color);
		DebugDrawSystem::DrawLine(scene, cornersInWorld[2 + i * 4], cornersInWorld[3 + i * 4], color);
		DebugDrawSystem::DrawLine(scene, cornersInWorld[0 + i * 4], cornersInWorld[2 + i * 4], color);
		DebugDrawSystem::DrawLine(scene, cornersInWorld[1 + i * 4], cornersInWorld[3 + i * 4], color);
	}
}

void GLRenderingDevice::FillDirLightQueue(SceneView& sceneView, const Dynarray<MeshRenderingComponent*>& meshCmps)
{
	ASSERTE(sceneView.DirectionalLights.GetSize() > 0, "FillDirLightQueue when scene view has no directional lights");
	DirectionalLightComponent* dirLight = sceneView.DirectionalLights[0];
	
	const CameraComponent* cameraCmp = sceneView.CameraCmp;
	Frustum frustum = cameraCmp->GetCameraFrustum();

	Matrix clipFromView;
	clipFromView.SetPerspective(frustum.GetFov(), frustum.GetAspect(), frustum.GetZNear(), frustum.GetZFar());
	// Matrix viewFromWorld = dirLight->DebugFrustumWorldFromModel.GetInversed();
	Matrix viewFromWorld = cameraCmp->GetViewFromWorld();
	Matrix clipFromWorld = clipFromView * viewFromWorld;

	// Transform frustum corners to DirLightSpace
	Dynarray<Vector> cornersInNDC{
		Vector(-1.0f,  1.0f, -1.0f), // back  left	top
		Vector( 1.0f,  1.0f, -1.0f), // back  right top
		Vector(-1.0f, -1.0f, -1.0f), // back  left  bot
		Vector( 1.0f, -1.0f, -1.0f), // back  right bot
		Vector(-1.0f,  1.0f,  1.0f), // front left	top
		Vector( 1.0f,  1.0f,  1.0f), // front right top
		Vector(-1.0f, -1.0f,  1.0f), // front left  bot
		Vector( 1.0f, -1.0f,  1.0f)	 // front right bot
	};

	// Transform frustum corners from NDC to World
	// could be done in one iteration but we need to do perspective division by W
	Matrix worldFromClip = clipFromWorld.GetInversed();
	Dynarray<Vector> cornersInWS;
	for (Vector posInClip : cornersInNDC)
	{
		Vector posInWS = worldFromClip * posInClip;
		posInWS.X /= posInWS.W;
		posInWS.Y /= posInWS.W;
		posInWS.Z /= posInWS.W;
		cornersInWS.PushBack(posInWS);
	}
	DrawFrustumPoints(sceneView.SceneData, cornersInWS, Color::RED);

	// based on https://mynameismjp.wordpress.com/2013/09/10/shadow-maps/
	// Stabilize shadow map: calculate sphere bounds around frustum to minimize AABB changes on frustum rotation 
	// Calculate the centroid of the view frustum slice
	Vector frustumCenterInWS;
	for (Vector posInWS : cornersInWS)
	{
		frustumCenterInWS += posInWS;
	}
	frustumCenterInWS *= 1.0f / 8.0f;

	float maxRadiusInWS = 0.0f;
	for (Vector posInWS : cornersInWS)
	{
		float radius = (cornersInWS[0] - posInWS).Length();
		maxRadiusInWS = std::max(maxRadiusInWS, radius);
	}
	maxRadiusInWS = std::ceil(maxRadiusInWS * 16.0f) / 16.0f; // MJP version

	Vector frustumMinInWS = frustumCenterInWS - Vector::ONE * maxRadiusInWS;
	Vector frustumMaxInWS = frustumCenterInWS + Vector::ONE * maxRadiusInWS;
	AABox frustumAABBInWS(frustumMinInWS, frustumMaxInWS - frustumMinInWS);

	Vector lightForward = dirLight->GetTransform().GetGlobalForward();
	Vector lightUp = dirLight->GetTransform().GetGlobalUp();
	Matrix lightFromWorld = Matrix(Vector::ZERO, lightForward, lightUp);
	Matrix worldFromLight = lightFromWorld.GetInversed();

	Vector frustumCenterInLS;
	for (Vector posInWS : cornersInWS)
	{
		frustumCenterInLS += lightFromWorld * posInWS;
	}
	frustumCenterInLS *= 1.0f / 8.0f;

	float maxRadiusInLS = 0.0f;
	for (Vector posInWS : cornersInWS)
	{
		float radius = (lightFromWorld * cornersInWS[0] - lightFromWorld * posInWS).Length();
		maxRadiusInLS = std::max(maxRadiusInLS, radius);
	}
	maxRadiusInLS = std::ceil(maxRadiusInLS * 16.0f) / 16.0f; // MJP version

	Vector frustumMinInLS = frustumCenterInLS - Vector::ONE * maxRadiusInLS;
	Vector frustumMaxInLS = frustumCenterInLS + Vector::ONE * maxRadiusInLS;
	AABox frustumAABBInLS(frustumMinInLS, frustumMaxInLS - frustumMinInLS);

	// if (frustumShowBounds)
	// {
		DebugDrawSystem::DrawSphere(sceneView.SceneData, frustumCenterInWS, maxRadiusInLS, Color::RED);
		DebugDrawSystem::DrawBox(sceneView.SceneData, frustumMinInWS, frustumMaxInWS, Color::RED*0.5f);
		DebugDrawSystem::DrawSphere(sceneView.SceneData, frustumCenterInWS, 5.0f, Color::RED*0.25f);
		DebugDrawSystem::DrawBox(sceneView.SceneData, frustumAABBInLS.GetMin(), frustumAABBInLS.GetMax(), worldFromLight, Color(1.0f, 1.0f, 0.0f));
	// }

	// if (sunShowAxes)
	// {
		DebugDrawSystem::DrawLine(sceneView.SceneData, frustumCenterInWS, frustumCenterInWS + dirLight->GetTransform().GetGlobalForward() * 50.0f, Color::BLACK);
		DebugDrawSystem::DrawLine(sceneView.SceneData, frustumCenterInWS, frustumCenterInWS + (dirLight->GetTransform().GetWorldFromModel() * Vector::UNIT_X) * 25.0f, Color::RED);
		DebugDrawSystem::DrawLine(sceneView.SceneData, frustumCenterInWS, frustumCenterInWS + (dirLight->GetTransform().GetWorldFromModel() * Vector::UNIT_Y) * 25.0f, Color::GREEN);
		DebugDrawSystem::DrawLine(sceneView.SceneData, frustumCenterInWS, frustumCenterInWS + (dirLight->GetTransform().GetWorldFromModel() * Vector::UNIT_Z) * 25.0f, Color::BLUE);
	// }

	FindShadowCasters(sceneView.SceneData, lightFromWorld, worldFromLight, frustumAABBInLS, true);

	// if (frustumShowBounds)
	// {
		DebugDrawSystem::DrawBox(sceneView.SceneData, frustumAABBInLS.GetMin(), frustumAABBInLS.GetMax(), lightFromWorld.GetInversed(), Color(1.0f, 1.0f, 0.0f));
	// }

	// dirLight->DebugShadowCenterInWS = frustumCenterInWS;
	// dirLight->ShadowAABBInWS = frustumAABBInWS;
	sceneView.DirShadowAABBInLS = frustumAABBInLS;
	// dirLight->ShadowAABBInLS = frustumAABBInLS;
}

void GLRenderingDevice::FindShadowCasters(Scene* scene, const Matrix& dirLightFromWorld, const Matrix& worldFromDirLight, AABox& frustumAABBInLS, bool drawBounds)
{
	const float maxFloat = std::numeric_limits<float>::max();

	Dynarray<MeshRenderingComponent*> meshCmps;
	for (auto [meshCmp] : scene->IterateComponents<MeshRenderingComponent>())
	{
		meshCmp->IsShadowCaster = false;
		meshCmps.PushBack(meshCmp);
	}
	// transform meshes AABB to DirLightSpace
	Dynarray<std::tuple<AABox, MeshRenderingComponent*>> boxMeshes;
	for (const auto meshCmp : meshCmps)
	{
		const Matrix& dirLightFromModel = dirLightFromWorld * meshCmp->GetTransform().GetWorldFromModel();
		Optional<AABox> boxWSOptional = meshCmp->GetBoundingBox(eEntityBoundingChannel::RENDERING);
		if (boxWSOptional.HasValue())
		{
			AABox boxWS = boxWSOptional.Value();
			AABox boxLS = boxWS.GetTransformed(dirLightFromModel);
			boxMeshes.PushBack(std::tuple(boxLS, meshCmp));
			if (drawBounds) DebugDrawSystem::DrawBox(scene, boxLS.GetMin(), boxLS.GetMax(), worldFromDirLight, Color::WHITE);
		}
	}

	// find min Y for near clipping plane and max Y for far clipping plane
	float maxZ = -maxFloat;
	for (const auto& [boxLS, mesh] : boxMeshes)
	{
		// extend dir light AAbox only in Z based on objects in rect defined on dir light AABob XY plane
		if (boxLS.OverlapsXY(frustumAABBInLS))
			maxZ = std::max(maxZ, boxLS.GetMax().Z);
	}

	if (maxZ > frustumAABBInLS.GetMax().Z)
	{
		Vector center = frustumAABBInLS.GetCenter(); // X and Z should be neutral so AABB expanded only on Y axis
		frustumAABBInLS.Expand(Vector(center.X, center.Y, maxZ));
	}

	if (drawBounds) DebugDrawSystem::DrawBox(scene, frustumAABBInLS.GetMin(), frustumAABBInLS.GetMax(), worldFromDirLight, Color(0.5f, 0.5f, 0.0f));

	// find all meshes that are inside extended DirLights AABB box
	int shadowCastersCounter = 0;
	for (auto& [box, meshCmp] : boxMeshes)
	{
		if (frustumAABBInLS.Overlaps(box))
		{
			meshCmp->IsShadowCaster = true;
			shadowCastersCounter++;
			if (drawBounds) DebugDrawSystem::DrawBox(scene, box.GetMin(), box.GetMax(), worldFromDirLight, Color::GREEN);
		}
	}

	gConsole.LogInfo("GLRenderingDevice::FillDirLightQueue casters: {}/{}", shadowCastersCounter, meshCmps.GetSize());
}

void GLRenderingDevice::CreateUtilityTextures()
{
	gConsole.LogInfo("GLRenderingDevice::CreateUtilityTextures");

	GLubyte dataWhite[] = { 255, 255, 255, 255 };
	glGenTextures(1, &FallbackWhiteTexture);
	glBindTexture(GL_TEXTURE_2D, FallbackWhiteTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, dataWhite);
	CHECK_GL_ERR();

	GLubyte dataBlack[] = { 0, 0, 0, 0 };
	glGenTextures(1, &FallbackBlackTexture);
	glBindTexture(GL_TEXTURE_2D, FallbackBlackTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, dataBlack);
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
