#pragma once

#include <ECS/ComponentBase.hpp>

using namespace Poly;

class GAME_DLLEXPORT KeyFrameAnimationComponent : public ComponentBase
{
public :

	KeyFrameAnimationComponent() {};

	String name;
	float AnimProgress;
	
};