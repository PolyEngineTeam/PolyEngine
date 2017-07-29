#include "LevelComponent.hpp"

#include <ResourceManager.hpp>

using namespace SGJ;

LevelComponent::LevelComponent(const Poly::String& path)
{
	Level = new LevelResource(path);
}
