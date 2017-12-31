#pragma once
#include "ComponentBase.hpp"
#include "DebugConfig.hpp"
#include "Dynarray.hpp"
#include "Vector.hpp"

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
			Mesh::Vector3D Begin;
			Mesh::Vector3D End;
		};

		Dynarray<DebugLine> DebugLines;
		Dynarray<DebugLine> DebugLinesColors;
	};

	REGISTER_COMPONENT(WorldComponentsIDGroup, DebugDrawLinesComponent)
}