#pragma once

#include <Core.hpp>
#include <Engine.hpp>
#include <World.hpp>

DECLARE_GAME()

enum class eGameComponents
{
	GROUND = (int)Poly::eEngineComponents::_COUNT,
	OBSTACLE,
	PLAYER,
	_COUNT
};

enum class eGameWorldComponents
{
	GAME_MGR = (int)Poly::eEngineWorldComponents::_COUNT,
	GAME_DEBUG,
	_COUNT
};

class GAME_DLLEXPORT SGJGame : public Poly::IGame {
public:
	void RegisterEngine(Poly::Engine* engine) override { }
	
	void Init() override;
	void Deinit() override;
};