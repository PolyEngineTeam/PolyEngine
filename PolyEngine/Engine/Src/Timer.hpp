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
		Timer(bool isPausable = false, double multiplier = 1);

		double GetTime() const { return Time.count(); }
		double GetMultiplier() const { return Multiplier; }
		double GetDeltaTime() const { return DeltaTime; }

	private:
		double DeltaTime;
		double Multiplier;
		std::chrono::duration<double> Time;

		bool IsPausable;

	};
}