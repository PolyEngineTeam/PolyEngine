#pragma once

#include <Defines.hpp>
#include <Time/TimeSystem.hpp>

namespace Poly
{

	class ENGINE_DLLEXPORT Timer : public BaseObject<>
	{
		friend void TimeSystem::TimeUpdatePhase(Scene * world);
	public:
		//------------------------------------------------------------------------------
		Timer(bool isPausable = false, double multiplier = 1) :
			Multiplier(multiplier),
			IsPausable(isPausable){}

		void SetMultiplier(double multiplier) { Multiplier = multiplier; }
		double GetTime() const { return Time.count(); }
		double GetMultiplier() const { return Multiplier; }
		double GetDeltaTime() const { return DeltaTime; }

	private:
		double DeltaTime = 0;
		double Multiplier = 1;
		std::chrono::duration<double> Time = std::chrono::duration<double>::zero();

		bool IsPausable = false;

	};
}
