#include "TestGame.hpp"

void TestGame::Init() 
{
	auto ent = Engine->GetWorld().SpawnEntity();
	Engine->GetWorld().AddComponent<Poly::TransformComponent>(ent);
	Engine->GetWorld().AddComponent<Poly::BaseCameraComponent>(ent, 45.0f, 1.0f, 1.0f, 1000.f);
};