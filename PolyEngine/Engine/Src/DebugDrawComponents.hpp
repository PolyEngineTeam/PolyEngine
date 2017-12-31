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

	class ENGINE_DLLEXPORT DebugRenderingPointsComponent : public ComponentBase
	{
	public:
		DebugRenderingPointsComponent()
		{ }

		struct DebugPoint
		{
			Vector Position;
			Vector Color;
			float Size;
		};

		Dynarray<DebugPoint> DebugPoints;
	};

	class ENGINE_DLLEXPORT DebugRenderingLinesComponent : public ComponentBase
	{
	public:
		DebugRenderingLinesComponent()
		{ }

		struct DebugLine
		{
			Mesh::Vector3D Begin;
			Mesh::Vector3D End;
		};

		Dynarray<DebugLine> DebugLines;
		Dynarray<DebugLine> DebugLinesColors;
	};

	REGISTER_COMPONENT(WorldComponentsIDGroup, DebugRenderingLinesComponent)

	class ENGINE_DLLEXPORT DebugRenderingStringsComponent : public ComponentBase
	{
	public:
		DebugRenderingStringsComponent()
		{ }

		struct DebugString
		{
			Vector Position; // in 2D orientation (Y axis is 0-ed)
			Vector Color;
			float FontSize;
		};

		Dynarray<DebugString> DebugStrings;
	};
}