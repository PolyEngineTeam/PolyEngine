#pragma once

#include <CameraComponent.hpp>
#include <TransformComponent.hpp>
#include <MeshRenderingComponent.hpp>
#include <FreeFloatMovementComponent.hpp>
#include <Core.hpp>
#include <Engine.hpp>
#include <World.hpp>

using namespace Poly;

DECLARE_GAME()

class GAME_DLLEXPORT RenderingSandbox : public IGame {
public:
	void RegisterEngine(Engine* engine) override { Engine = engine; }
	
	void Init() override;
	void Deinit() override;

private:
	
	Engine* Engine;
};
