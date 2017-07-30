#pragma once

#include <Core.hpp>
#include <Engine.hpp>
#include <World.hpp>

DECLARE_GAME()

enum class eGameComponents
{
	TILE = (int)Poly::eEngineComponents::_COUNT,
	PLAYER,
	_COUNT
};

enum class eGameWorldComponents
{
	GAME_MGR = (int)Poly::eEngineWorldComponents::_COUNT,
	GAME_DEBUG,
	_COUNT
};

namespace Poly { class SoundResource; }

class GAME_DLLEXPORT SGJGame : public Poly::IGame {
public:
	void RegisterEngine(Poly::Engine* engine) override { }
	
	void Init() override;
	void Deinit() override;

private:
	Poly::SoundResource* JumpSoundPreload;
};