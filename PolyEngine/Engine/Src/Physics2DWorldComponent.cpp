#include "EnginePCH.hpp"

#include "Physics2DWorldComponent.hpp"
#include <Box2D/Box2D.h>

using namespace Poly;

Physics2DWorldComponent::Physics2DWorldComponent(const Physics2DConfig& config)
	: Config(config), World(std::make_unique<b2World>(b2Vec2(config.Gravity.X, config.Gravity.Y)))
{
}

Poly::Physics2DWorldComponent::~Physics2DWorldComponent()
{
}
