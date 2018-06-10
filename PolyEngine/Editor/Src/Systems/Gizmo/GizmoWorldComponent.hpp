#pragma once

#include "ECS/ComponentBase.hpp"

using namespace Poly;

class GizmoWorldComponent : public ComponentBase
{
	RTTI_DECLARE_TYPE_DERIVED(GizmoWorldComponent, ComponentBase)
	{
		NO_RTTI_PROPERTY()
	}

public:
	GizmoWorldComponent(); 

private:

};