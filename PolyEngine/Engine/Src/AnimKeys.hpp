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

			AnimKeys(Dynarray<Vector>& positions, Dynarray<Quaternion>& rotations, Dynarray<Vector>& scales)
				: Positions(positions), Rotations(rotations), Scales(scales)
			{
				gConsole.LogDebug("positions size: {}", positions.GetSize());
				gConsole.LogDebug("rotations size: {}", rotations.GetSize());
				gConsole.LogDebug("scales size: {}", scales.GetSize());
				ASSERTE(Positions.GetSize() == Rotations.GetSize() && Positions.GetSize() == Scales.GetSize(), "size of key arrays missmatch!");
			}


			Dynarray<Vector> Positions;
			Dynarray<Quaternion> Rotations;
			Dynarray<Vector> Scales;
	};
}