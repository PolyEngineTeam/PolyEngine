#pragma once

#include <CameraComponent.hpp>
#include <TransformComponent.hpp>
#include <MeshRenderingComponent.hpp>
#include <FreeFloatMovementComponent.hpp>
#include <Core.hpp>
#include <Engine.hpp>
#include <World.hpp>

using namespace Poly;

namespace GameMainSystem
{
	void GameUpdate(World* world);
}


enum class eGameWorldComponents
{
	GAME_MGR = (int)eEngineWorldComponents::_COUNT,
	_COUNT
};

DECLARE_GAME()

class GAME_DLLEXPORT RenderingSandbox : public IGame {
public:
	void RegisterEngine(Engine* engine) override { Engine = engine; }
	
	void Init() override;
	void GameUpdate(World* world);
	void Deinit() override;

private:
	UniqueID Camera;
	UniqueID GameManager;
	UniqueID Dummy;
	UniqueID PointLight;

	Dynarray<UniqueID> GameEntities;
	Engine* Engine;

	// float Random();
	// float Random(float min, float max);
	// void AddDirectionalLights();
	// void AddPointLights(int Quata);
	// void CreatePointLight(float Range);
	// void CreateSpotLight(float Range);
};
