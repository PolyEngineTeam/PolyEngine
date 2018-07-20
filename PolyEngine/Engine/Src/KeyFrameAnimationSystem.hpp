#pragma once

#include <AnimKeys.hpp>
#include <Math/Vector.hpp>
#include <Math/Quaternion.hpp>
#include <Collections/String.hpp>

namespace Poly
{
	class Scene;

	namespace KeyFrameAnimationSystem
	{
		void ENGINE_DLLEXPORT KeyFrameAnimationUpdatePhase(Scene* scene);

		OrderedMap<String, AnimKeys> ENGINE_DLLEXPORT LoadAnimTracks(String path);
		Vector ENGINE_DLLEXPORT AnimTrack_ReadVector3FromRow(String row);
		Quaternion ENGINE_DLLEXPORT AnimTrack_ReadQuternion4FromRow(String row);
	}
}