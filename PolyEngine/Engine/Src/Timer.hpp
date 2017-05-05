#pragma once

#include <chrono>
#include <Core.hpp>
#include "TimeSystem.hpp"

using std::chrono::duration;
using std::chrono::steady_clock;
using std::chrono::duration_cast;

namespace Poly
{

	class ENGINE_DLLEXPORT Timer : public BaseObject<>
	{
		friend void TimeSystem::TimeUpdatePhase(World * world);
	public:
		Timer(bool isPausable);

		float GetTime() const { return Time.count(); };

	private:
		duration<float> Time;

		bool IsPausable;

	};
}