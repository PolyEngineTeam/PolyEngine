#pragma once

#include <Core.hpp>
#include <Engine.hpp>
#include <World.hpp>

namespace GameMainSystem
{
	void GameUpdate(Poly::World* world);
}

enum class eGameComponents
{
	PLAYERCONTROLLER = (int)Poly::eEngineComponents::_COUNT
};

class GAME_DLLEXPORT InvadersGame : public Poly::IGame {
public:
	void RegisterEngine(Poly::Engine* engine) override { Engine = engine; }
	
	void Init() override;
	void Deinit() override;

private:
	Poly::UniqueID Camera;

	Poly::Dynarray<Poly::UniqueID> GameEntities;
	Poly::Engine* Engine;
};