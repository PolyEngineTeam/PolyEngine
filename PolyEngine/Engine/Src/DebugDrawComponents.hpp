#pragma once
#include "ComponentBase.hpp"
#include "DebugConfig.hpp"
#include "Dynarray.hpp"
#include "Color.hpp"
#include "Vector3f.hpp"

namespace Poly
{
	/// Component used for labeling entities for which the debug primitives 
	/// will be drawn.
	class ENGINE_DLLEXPORT DebugDrawableComponent : public ComponentBase
	{
	public:
		DebugDrawableComponent(DebugDrawPreset preset) : entityPreset(preset) { };

		const DebugDrawPreset entityPreset;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, DebugDrawableComponent)

	class ENGINE_DLLEXPORT DebugDrawLinesComponent : public ComponentBase
	{
	public:
		DebugDrawLinesComponent()
		{ }

		struct DebugLine
		{
			Vector3f Begin;
			Vector3f End;
		};

		struct DebugLineColor
		{
			Color Begin;
			Color End;
		};

		Dynarray<DebugLine> DebugLines;
		Dynarray<DebugLineColor> DebugLinesColors;
	};

	REGISTER_COMPONENT(WorldComponentsIDGroup, DebugDrawLinesComponent)
}