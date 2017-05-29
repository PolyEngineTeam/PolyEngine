#include "EnginePCH.hpp"

#include "Timer.hpp"

//------------------------------------------------------------------------------
Poly::Timer::Timer(bool isPausable, double multiplier) 
	: IsPausable(isPausable),
	Multiplier(multiplier)
{
	Time = std::chrono::duration<double>::zero();
}
