#pragma once
#include "ComponentBase.hpp"
#include "Dynarray.hpp"
#include "Vector.hpp"

namespace Poly
{
	class ENGINE_DLLEXPORT DebugRenderingPointsComponent : public ComponentBase
	{
	public:
		DebugRenderingPointsComponent();

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
		DebugRenderingLinesComponent();

		struct DebugLine
		{
			Vector Begin;
			Vector End;
			Vector Color;
		};

		Dynarray<DebugLine> DebugLines;
	};

	class ENGINE_DLLEXPORT DebugRenderingStringsComponent : public ComponentBase
	{
	public:
		DebugRenderingStringsComponent();

		struct DebugString
		{
			Vector Position; // in 2D orientation (Y axis is 0-ed)
			Vector Color;
			float FontSize;
		};

		Dynarray<DebugString> DebugStrings;
	};
}