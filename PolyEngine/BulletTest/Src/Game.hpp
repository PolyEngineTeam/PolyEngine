#pragma once

#include <Core.hpp>
#include <Engine.hpp>
#include <World.hpp>

DECLARE_GAME()

class GAME_DLLEXPORT BulletTest : public Poly::IGame 
{
public:
	void RegisterEngine(Poly::Engine* /*engine*/) override { }

	void Init() override;
	void Deinit() override;
};
