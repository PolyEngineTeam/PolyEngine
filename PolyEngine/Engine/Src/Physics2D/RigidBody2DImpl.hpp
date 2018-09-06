#pragma once

#include <Defines.hpp>

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
