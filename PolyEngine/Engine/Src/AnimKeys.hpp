#pragma once

#include "EnginePCH.hpp"
#include <Collections/Dynarray.hpp>
#include <Math/Vector.hpp>

namespace Poly
{
	class ENGINE_DLLEXPORT AnimKeys : public BaseObject<>
	{
		public:
			AnimKeys()
			{}

			AnimKeys(Dynarray<Vector>& positions, Dynarray<Quaternion>& rotations, Dynarray<Vector>& scales, int ticksPerSecond)
				: Positions(positions), Rotations(rotations), Scales(scales), TicksPerSecond(ticksPerSecond)
			{
				ASSERTE(Positions.GetSize() == Rotations.GetSize() && Positions.GetSize() == Scales.GetSize(), "size of key arrays missmatch!");
			}

			Dynarray<Vector> Positions;
			Dynarray<Quaternion> Rotations;
			Dynarray<Vector> Scales;
			int TicksPerSecond;
	};
}