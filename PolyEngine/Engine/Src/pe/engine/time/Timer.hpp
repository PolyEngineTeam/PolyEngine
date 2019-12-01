#pragma once

#include <pe/Defines.hpp>
#include <pe/engine/time/TimeSystem.hpp>

namespace pe::engine::time
{

	class ENGINE_DLLEXPORT Timer : public ::pe::core::BaseObject<>
	{
		friend class TimeSystem;
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
