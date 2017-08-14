#pragma once

#include <Box2D/Box2D.h>

using namespace Poly;

namespace Poly {
	struct RigidBody2DData
	{
		b2BodyDef BodyDef;
		b2FixtureDef FixtureDef;
		b2Body* Body = nullptr;
		b2Fixture* Fixture = nullptr;
	};
}
