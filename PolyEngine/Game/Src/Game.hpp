#pragma once

#include <Core.hpp>
#include <Engine.hpp>
#include <World.hpp>

DECLARE_GAME()

enum class eGameComponents
{
	BACKGROUND = (int)Poly::eEngineComponents::_COUNT,
	_COUNT
};

enum class eGameWorldComponents
{
	GAME_MGR = (int)Poly::eEngineWorldComponents::_COUNT,
	_COUNT
};

class GAME_DLLEXPORT SGJGame : public Poly::IGame {
public:
	void RegisterEngine(Poly::Engine* engine) override { }
	
	void Init() override;
	void Deinit() override;
};