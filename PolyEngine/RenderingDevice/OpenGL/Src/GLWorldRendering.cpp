#include <PolyRenderingDeviceGLPCH.hpp>

#include <GLRenderingDevice.hpp>

#include <algorithm>    // std::min

#include <Proxy/GLTextFieldBufferDeviceProxy.hpp>
#include <Proxy/GLTextureDeviceProxy.hpp>
#include <Pipeline/RenderingPassBase.hpp>
#include <Proxy/GLMeshDeviceProxy.hpp>
#include <Common/GLUtils.hpp>
#include <Common/PrimitiveCube.hpp>
#include <Common/PrimitiveQuad.hpp>
#include <Debugging/DebugDrawSystem.hpp>
#include <ForwardRenderer.hpp>
#include <TiledForwardRenderer.hpp>

using namespace Poly;
using MeshQueue = core::storage::PriorityQueue<const MeshRenderingComponent*, SceneView::DistanceToCameraComparator>;

void GLRenderingDevice::Init()
{
	core::utils::gConsole.LogInfo("GLRenderingDevice::Init");

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
	for (auto& [id, viewport] : world->GetWorldComponent<ViewportWorldComponent>()->GetViewports())
	{
		SceneView sceneView(world, viewport);
		FillSceneView(sceneView);
		Renderer->Render(sceneView);
	}

	// Signal frame end
	EndFrame();
}

void GLRenderingDevice::FillSceneView(SceneView& sceneView)
{
	std::vector<const MeshRenderingComponent*> meshCmps;
	for (const auto& [meshCmp] : sceneView.SceneData->IterateComponents<MeshRenderingComponent>())
	{
		meshCmps.push_back(meshCmp);
	}

	for (const auto& meshCmp : meshCmps)
	{
		if (sceneView.CameraCmp->IsVisibleToCamera(meshCmp->GetOwner()))
		{
			if (meshCmp->GetBlendingMode() == eBlendingMode::OPAUQE)
			{
				sceneView.OpaqueQueue.Push(meshCmp);
			}
			else if (meshCmp->GetBlendingMode() == eBlendingMode::TRANSLUCENT)
			{
				sceneView.TranslucentQueue.Push(meshCmp);
			}
		}
	}

	for (const auto& [particleCmp] : sceneView.SceneData->IterateComponents<ParticleComponent>())
	{
		sceneView.ParticleQueue.Push(particleCmp);
	}

	for (const auto& [dirLightCmp] : sceneView.SceneData->IterateComponents<DirectionalLightComponent>())
	{
		sceneView.DirectionalLightList.push_back(dirLightCmp);
	}

	for (const auto& [pointLightCmp] : sceneView.SceneData->IterateComponents<PointLightComponent>())
	{
		sceneView.PointLightList.push_back(pointLightCmp);
	}

	if (sceneView.DirectionalLightList.size() > 0)
		CullDirLightQueue(sceneView, meshCmps);
}

void GLRenderingDevice::CullDirLightQueue(SceneView& sceneView, const std::vector<const MeshRenderingComponent*>& meshCmps)
{
	const DirectionalLightComponent* dirLight = sceneView.DirectionalLightList[0];
	const CameraComponent* cameraCmp = sceneView.CameraCmp;
	core::math::Frustum frustum = cameraCmp->GetCameraFrustum();

	core::math::Matrix clipFromView;
	clipFromView.SetPerspective(frustum.GetFov(), frustum.GetAspect(), frustum.GetZNear(), frustum.GetZFar());
	core::math::Matrix viewFromWorld = cameraCmp->GetViewFromWorld();
	core::math::Matrix clipFromWorld = clipFromView * viewFromWorld;

	// Transform frustum corners to DirLightSpace
	std::vector<::pe::core::math::Vector> cornersInNDC {
		core::math::Vector(-1.0f,  1.0f, -1.0f), // back  left	top
		core::math::Vector( 1.0f,  1.0f, -1.0f), // back  right top
		core::math::Vector(-1.0f, -1.0f, -1.0f), // back  left  bot
		core::math::Vector( 1.0f, -1.0f, -1.0f), // back  right bot
		core::math::Vector(-1.0f,  1.0f,  1.0f), // front left	top
		core::math::Vector( 1.0f,  1.0f,  1.0f), // front right top
		core::math::Vector(-1.0f, -1.0f,  1.0f), // front left  bot
		core::math::Vector( 1.0f, -1.0f,  1.0f)	 // front right bot
	};

	// Transform frustum corners from NDC to World
	// could be done in one iteration but we need to do perspective division by W
	core::math::Matrix worldFromClip = clipFromWorld.GetInversed();
	std::vector<::pe::core::math::Vector> cornersInWS;
	for (::pe::core::math::Vector posInClip : cornersInNDC)
	{
		core::math::Vector posInWS = worldFromClip * posInClip;
		posInWS.X /= posInWS.W;
		posInWS.Y /= posInWS.W;
		posInWS.Z /= posInWS.W;
		cornersInWS.push_back(posInWS);
	}
	DebugDrawSystem::DrawFrustumPoints(sceneView.SceneData, cornersInWS, core::math::Color::RED);

	// based on https://mynameismjp.wordpress.com/2013/09/10/shadow-maps/
	// Stabilize shadow map: calculate sphere bounds around frustum to minimize AABB changes on frustum rotation 
	// Calculate the centroid of the view frustum slice
	core::math::Vector lightForward = dirLight->GetTransform().GetGlobalForward();
	core::math::Vector lightUp = dirLight->GetTransform().GetGlobalUp();
	core::math::Matrix lightFromWorld = core::math::Matrix(::pe::core::math::Vector::ZERO, lightForward, lightUp);
	// @fixme: Transpose is needed to correctly multiply light rotation
	// (created with look at) with rest of light projection matrices.
	// Same rotation is created when inverted axes are passed to look at constructor
	lightFromWorld.Transpose();
	core::math::Matrix worldFromLight = lightFromWorld.GetInversed();

	core::math::Vector frustumCenterInLS;
	for (::pe::core::math::Vector posInWS : cornersInWS)
	{
		frustumCenterInLS += lightFromWorld * posInWS;
	}
	frustumCenterInLS *= 1.0f / 8.0f;

	float maxRadiusInLS = 0.0f;
	for (::pe::core::math::Vector posInWS : cornersInWS)
	{
		float radius = (lightFromWorld * cornersInWS[0] - lightFromWorld * posInWS).Length();
		maxRadiusInLS = std::max(maxRadiusInLS, radius);
	}
	maxRadiusInLS = std::ceil(maxRadiusInLS * 16.0f) / 16.0f; // MJP version

	core::math::Vector frustumMinInLS = frustumCenterInLS - core::math::Vector::ONE * maxRadiusInLS;
	core::math::Vector frustumMaxInLS = frustumCenterInLS + core::math::Vector::ONE * maxRadiusInLS;
	core::math::AABox frustumAABBInLS(frustumMinInLS, frustumMaxInLS - frustumMinInLS);

	if (sceneView.SettingsCmp && sceneView.SettingsCmp->DebugDrawFrustumBounds)
		DebugDrawSystem::DrawBox(sceneView.SceneData, frustumAABBInLS.GetMin(), frustumAABBInLS.GetMax(), worldFromLight, core::math::Color(1.0f, 1.0f, 0.0f));

	CullShadowCasters(sceneView, lightFromWorld, worldFromLight, frustumAABBInLS);

	if (sceneView.SettingsCmp && sceneView.SettingsCmp->DebugDrawFrustumBounds)
		DebugDrawSystem::DrawBox(sceneView.SceneData, frustumAABBInLS.GetMin(), frustumAABBInLS.GetMax(), lightFromWorld.GetInversed(), core::math::Color(1.0f, 1.0f, 0.0f));

	sceneView.DirShadowAABBInLS = frustumAABBInLS;
}

void GLRenderingDevice::CullShadowCasters(SceneView& sceneView, const core::math::Matrix& dirLightFromWorld, const core::math::Matrix& worldFromDirLight, core::math::AABox& frustumAABBInLS)
{
	const float maxFloat = std::numeric_limits<float>::max();
	Scene* scene = sceneView.SceneData;

	std::vector<MeshRenderingComponent*> meshCmps;
	for (auto [meshCmp] : scene->IterateComponents<MeshRenderingComponent>())
	{
		meshCmps.push_back(meshCmp);
	}
	// transform meshes AABB to DirLightSpace
	std::vector<std::tuple<::pe::core::math::AABox, MeshRenderingComponent*>> boxMeshes;
	for (const auto meshCmp : meshCmps)
	{
		const core::math::Matrix& dirLightFromModel = dirLightFromWorld * meshCmp->GetTransform().GetWorldFromModel();
		std::optional<::pe::core::math::AABox> boxWSOptional = meshCmp->GetBoundingBox(eEntityBoundingChannel::RENDERING);
		if (boxWSOptional.has_value())
		{
			core::math::AABox boxWS = boxWSOptional.value();
			core::math::AABox boxLS = boxWS.GetTransformed(dirLightFromModel);
			boxMeshes.push_back(std::tuple(boxLS, meshCmp));

			if (sceneView.SettingsCmp && sceneView.SettingsCmp->DebugDrawShadowCastersBounds)
				DebugDrawSystem::DrawBox(scene, boxLS.GetMin(), boxLS.GetMax(), worldFromDirLight, core::math::Color::WHITE);
		}
	}

	// find min Y for near clipping plane and max Y for far clipping plane
	float maxZ = -maxFloat;
	for (const auto& [boxLS, mesh] : boxMeshes)
	{
		// extend dir light AAbox only in Z based on objects in rect defined on dir light AABob XY plane
		if (boxLS.ContainsXY(frustumAABBInLS))
			maxZ = std::max(maxZ, boxLS.GetMax().Z);
	}

	if (maxZ > frustumAABBInLS.GetMax().Z)
	{
		core::math::Vector center = frustumAABBInLS.GetCenter(); // X and Z should be neutral so AABB expanded only on Y axis
		frustumAABBInLS.Expand(::pe::core::math::Vector(center.X, center.Y, maxZ));
	}

	if (sceneView.SettingsCmp && sceneView.SettingsCmp->DebugDrawShadowCastersBounds)
		DebugDrawSystem::DrawBox(scene, frustumAABBInLS.GetMin(), frustumAABBInLS.GetMax(), worldFromDirLight, core::math::Color(0.5f, 0.5f, 0.0f));
	

	// find all meshes that are inside extended DirLights AABB box
	core::math::Vector dirLightPos = sceneView.DirectionalLightList[0]->GetTransform().GetGlobalTranslation();
	MeshQueue shadowCasterQueue(SceneView::DistanceToCameraComparator(dirLightPos, SceneView::eSortOrderType::FRONT_TO_BACK), 0);
	
	for (auto& [box, meshCmp] : boxMeshes)
	{
		if (frustumAABBInLS.Contains(box))
		{
			shadowCasterQueue.Push(meshCmp);

			if (sceneView.SettingsCmp && sceneView.SettingsCmp->DebugDrawShadowCastersBounds)
				DebugDrawSystem::DrawBox(scene, box.GetMin(), box.GetMax(), worldFromDirLight, core::math::Color::GREEN);
		}
	}

	sceneView.DirShadowCastersQueue = std::move(shadowCasterQueue);

	// core::utils::gConsole.LogInfo("GLRenderingDevice::FillDirLightQueue casters: {}/{}", shadowCastersCounter, meshCmps.GetSize());
}

void GLRenderingDevice::CreateUtilityTextures()
{
	core::utils::gConsole.LogInfo("GLRenderingDevice::CreateUtilityTextures");

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

	core::math::Vector noise[16];
	for (int i = 0; i < 16; ++i) {
		noise[i] = core::math::Vector(
			core::math::RandomRange(-1.0f, 1.0f),
			core::math::RandomRange(-1.0f, 1.0f),
			0.0f
		);
		noise[i].Normalize();
	}

	GLubyte dataSSAONoise[16 * 3];
	for (int i = 0; i < 16; i += 3) {
		core::math::Vector positive = noise[i] * 0.5f + 0.5f;
		dataSSAONoise[i + 0] = (GLbyte)core::math::Clamp((int)(positive.X * 255.0f), 0, 255);
		dataSSAONoise[i + 1] = (GLbyte)core::math::Clamp((int)(positive.Y * 255.0f), 0, 255);
		dataSSAONoise[i + 2] = (GLbyte)core::math::Clamp((int)(positive.Z * 255.0f), 0, 255);
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
