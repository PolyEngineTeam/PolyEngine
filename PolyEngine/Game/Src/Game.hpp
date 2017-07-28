#pragma once

#include <Core.hpp>
#include <Engine.hpp>
#include <World.hpp>

namespace GameMainSystem
{
	void GameUpdate(Poly::World* world);
}

DECLARE_GAME()
class GAME_DLLEXPORT SGJGame : public Poly::IGame {
public:
	void RegisterEngine(Poly::Engine* engine) override { Engine = engine; }
	
	void Init() override;
	void Deinit() override;

private:
	Poly::UniqueID Camera;

	Poly::Dynarray<Poly::UniqueID> GameEntities;
	Poly::Engine* Engine;
};