#pragma once

#include <Core.hpp>
#include <Engine.hpp>
#include <World.hpp>

DECLARE_GAME()

namespace Poly { class SoundResource; }

class GAME_DLLEXPORT SGJGame : public Poly::IGame {
public:
	void RegisterEngine(Poly::Engine* /*engine*/) override { }

	void Init() override;
	void Deinit() override;

private:
	Poly::Dynarray<Poly::SoundResource*> SoundsPreload;
};
