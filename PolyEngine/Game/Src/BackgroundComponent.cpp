#include "BackgroundComponent.hpp"
#include "BackgroundComponent.hpp"

using namespace Poly;

Poly::BackgroundComponent::BackgroundComponent(double spawnTime) :
	SpawnTime(spawnTime)
{
	gConsole.LogInfo("Poly::BackgroundComponent::BackgroundComponent() spawnTime: {}", spawnTime);
}
