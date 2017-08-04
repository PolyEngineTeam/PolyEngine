#pragma once

#include <CameraComponent.hpp>
#include <TransformComponent.hpp>
#include <MeshRenderingComponent.hpp>
#include <FreeFloatMovementComponent.hpp>
#include <Core.hpp>
#include <Engine.hpp>
#include <World.hpp>

namespace GameMainSystem
{
	void GameUpdate(Poly::World* world);
}


DECLARE_GAME()
class GAME_DLLEXPORT RenderingSandbox : public Poly::IGame {
public:
	void RegisterEngine(Poly::Engine* engine) override { Engine = engine; }
	
	void Init() override;
	void Deinit() override;

private:
	Poly::UniqueID Camera;
	Poly::UniqueID GameManager;
	Poly::UniqueID Light;

	Poly::Dynarray<Poly::UniqueID> GameEntities;
	Poly::Engine* Engine;
};