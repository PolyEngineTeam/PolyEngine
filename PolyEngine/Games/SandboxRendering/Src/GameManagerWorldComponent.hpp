#pragma once

#include <LightSourceComponent.hpp>
#include <MeshRenderingComponent.hpp>
#include <ComponentBase.hpp>
#include <Dynarray.hpp>
#include <UniqueID.hpp>

using namespace Poly;

class GAME_DLLEXPORT GameManagerWorldComponent : public ComponentBase
{
public:
	UniqueID Camera;
	UniqueID KeyDirLight;
	Dynarray<UniqueID> DirLights;
	Dynarray<PointLightComponent*> PointLights;
	Dynarray<Vector> PointLightPositions;
	Dynarray<UniqueID> SpotLights;
	Dynarray<MeshRenderingComponent*> DebugMeshes;
	bool IsDrawingDebugMeshes = true;

	Dynarray<UniqueID> GameEntities;
};
