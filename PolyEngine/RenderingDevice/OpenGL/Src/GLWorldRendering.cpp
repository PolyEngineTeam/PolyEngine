#include "PolyRenderingDeviceGLPCH.hpp"

#include "GLRenderingDevice.hpp"

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
	for (const auto componentsTuple : sceneView.SceneData->IterateComponents<DirectionalLightComponent>())
	{
		sceneView.DirectionalLights.PushBack(std::get<DirectionalLightComponent*>(componentsTuple));
	}

	for (const auto componentsTuple : sceneView.SceneData->IterateComponents<PointLightComponent>())
	{
		sceneView.PointLights.PushBack(std::get<PointLightComponent*>(componentsTuple));
	}

	Dynarray<const MeshRenderingComponent*> meshCmps;
	for (const auto componentsTuple : sceneView.SceneData->IterateComponents<MeshRenderingComponent>())
	{
		meshCmps.PushBack(std::get<MeshRenderingComponent*>(componentsTuple));
	}

	for ( auto meshCmp : meshCmps)
	{
		// sceneView.DirShadowOpaqueQueue.PushBack(meshCmp);

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

	if (sceneView.DirectionalLights.GetSize() > 0)
	{
		FillDirLightQueue(sceneView, meshCmps);
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

void GLRenderingDevice::FillDirLightQueue(SceneView& sceneView, const Dynarray<const MeshRenderingComponent*>& meshCmps)
{
	// GameManagerWorldComponent* gameMgrCmp = scene->GetWorldComponent<GameManagerWorldComponent>();
	// DirectionalLightComponent* dirLight = gameMgrCmp->KeyDirLight.Get()->GetComponent<DirectionalLightComponent>();
	ASSERTE(sceneView.DirectionalLights.GetSize() > 0, "FillDirLightQueue when scene view has no directional lights");
	const DirectionalLightComponent* dirLight = sceneView.DirectionalLights[0];
	
	// // DEBUG: Recreate camera matrices for test
	// Vector position = Vector::UNIT_Z * 100.0f + Vector::UNIT_X * 2000.0f * Cos(40_deg * time);
	// Quaternion rotationQuat = Quaternion(Vector::UNIT_X, 120_deg * Sin(5_deg * time))
	// 	* Quaternion(Vector::UNIT_Y, 120_deg * Sin(-10_deg * time));
	
	Matrix clipFromView;
	clipFromView.SetPerspective(
		dirLight->DebugFrustum.GetFov(),
		dirLight->DebugFrustum.GetAspect(),
		dirLight->DebugFrustum.GetZNear(),
		dirLight->DebugFrustum.GetZFar()
	);
	Matrix viewFromWorld = dirLight->DebugFrustumWorldFromModel.GetInversed();
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
	Dynarray<Vector> cornersInWorld;
	for (Vector posInClip : cornersInNDC)
	{
		Vector world = worldFromClip * posInClip;
		world.X /= world.W;
		world.Y /= world.W;
		world.Z /= world.W;
		cornersInWorld.PushBack(world);
	}

	DrawFrustumPoints(sceneView.SceneData, cornersInWorld, Color::RED);

	// Transform frustum corners from World to DirLight
	Matrix worldFromDirLight = dirLight->GetTransform().GetWorldFromModel();
	Matrix dirLightFromWorld = worldFromDirLight.GetInversed();

	// find min and max corners and create AABB in DirLightSpace
	const float maxFloat = std::numeric_limits<float>::max();
	Vector min(maxFloat, maxFloat, maxFloat);
	Vector max(-maxFloat, -maxFloat, -maxFloat);

	Dynarray<Vector> cornersInDirLight;
	for (Vector posInWorld : cornersInWorld)
	{
		Vector posInDirLight = dirLightFromWorld * posInWorld;
		min = Vector::Min(min, posInDirLight);
		max = Vector::Max(max, posInDirLight);
	}

	AABox dirLightAABB(min, max - min);
	// DebugDrawSystem::DrawBox(sceneView.SceneData, min, max, worldFromDirLight, Color::BLUE);


	// transform meshes AABB to DirLightSpace
	Dynarray<std::tuple<AABox, const MeshRenderingComponent*>> meshBoxes;
	for (const auto& meshCmp : meshCmps)
	{
		const Matrix& dirLightFromModel = dirLightFromWorld * meshCmp->GetTransform().GetWorldFromModel();

		Optional<AABox> boxInLightOptional = meshCmp->GetBoundingBox(eEntityBoundingChannel::RENDERING);
		if (boxInLightOptional.HasValue())
		{
			AABox boxInLight = boxInLightOptional.Value();
			boxInLight = boxInLight.GetTransformed(dirLightFromModel);
			meshBoxes.PushBack(std::tuple(boxInLight, meshCmp));
			// DebugDrawSystem::DrawBox(sceneView.SceneData, boxInLight.GetMin(), boxInLight.GetMax(), worldFromDirLight, Color::WHITE);
		}
	}

	// find min Z for near clipping plane and max Z for far clipping plane
	float minZ = maxFloat;
	float maxZ = -maxFloat;
	for (const auto& kv : meshBoxes)
	{
		AABox box = std::get<0>(kv);
		if (!(box.IntersectsXY(dirLightAABB))) // extend dir light AAbox only in Z based on objects in rect defined on dir light AABob XY plane
			continue;

		minZ = std::min(minZ, box.GetMin().Z);
		maxZ = std::max(maxZ, box.GetMax().Z);
	}

	Vector center = dirLightAABB.GetCenter(); // X and Y should be neutral so AABB expanded only on Z axis
	dirLightAABB.Expand(Vector(center.X, center.Y, minZ))
				.Expand(Vector(center.X, center.Y, maxZ));

	DebugDrawSystem::DrawBox(sceneView.SceneData, dirLightAABB.GetMin(), dirLightAABB.GetMax(), worldFromDirLight, Color(1.0f, 1.0f, 0.0f));

	// find all meshes that are inside extended DirLights AABB box
	int shadowCastersCounter = 0;
	for (auto kv : meshBoxes)
	{
		AABox box = std::get<0>(kv);
		const MeshRenderingComponent* meshCmp = std::get<1>(kv);

		if (box.Intersects(dirLightAABB))
		{
			sceneView.DirShadowOpaqueQueue.PushBack(meshCmp);

			DebugDrawSystem::DrawBox(sceneView.SceneData, box.GetMin(), box.GetMax(), worldFromDirLight, Color::GREEN);
			shadowCastersCounter++;
		}
	}

	sceneView.ShadowAABB = dirLightAABB;

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
