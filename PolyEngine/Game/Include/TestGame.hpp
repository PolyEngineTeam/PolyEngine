#pragma once

#include <Core.hpp>
#include <Engine.hpp>

class DLLEXPORT TestGame : public Poly::IGame {
public:
	void RegisterEngine(Poly::Engine* engine) override { Engine = engine; }
	void Init() override {};
	void Update(float dt) override {};

private:
	Poly::Engine* Engine;
};