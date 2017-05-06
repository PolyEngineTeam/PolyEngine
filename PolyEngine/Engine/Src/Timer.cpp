#include "EnginePCH.hpp"

#include "Timer.hpp"

//------------------------------------------------------------------------------
Poly::Timer::Timer(bool isPausable) 
	: IsPausable(isPausable)
{
	Time = std::chrono::duration<float>::zero();
}
