#pragma once

#include <Core.hpp>
#include <Engine.hpp>
#include <World.hpp>

namespace GameMainSystem
{
	void GameUpdate(Poly::World* world);
}

DECLARE_GAME()

enum class eGameComponents
{
	LEVELCOMPONENT = (int)Poly::eEngineComponents::_COUNT,
	BACKGROUND,
	_COUNT
};

class GAME_DLLEXPORT SGJGame : public Poly::IGame {
public:
	void RegisterEngine(Poly::Engine* engine) override { Engine = engine; }
	
	void Init() override;
	void Deinit() override;

private:
	Poly::UniqueID Camera;
	Poly::UniqueID Background;

	Poly::Dynarray<Poly::UniqueID> GameEntities;
	Poly::Engine* Engine;
};