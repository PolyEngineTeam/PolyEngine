#pragma once

#include <chrono>
#include <Core.hpp>
#include "TimeSystem.hpp"

namespace Poly
{

	class ENGINE_DLLEXPORT Timer : public BaseObject<>
	{
		friend void TimeSystem::TimeUpdatePhase(World * world);
	public:
		Timer(bool isPausable = false);

		double GetTime() const { return Time.count(); };
		double GetDeltaTime() const { return DeltaTime; };

	private:
		double DeltaTime;
		std::chrono::duration<double> Time;

		bool IsPausable;

	};
}