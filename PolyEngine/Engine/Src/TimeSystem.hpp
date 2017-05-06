#pragma once


namespace Poly
{
	class World;

	namespace TimeSystem
	{
		void TimeUpdatePhase(World* world);

		void RegisterTimer(World* world, const size_t id, const bool isPausable);

		float GetDeltaTime(World* world);

		float GetTimerTime(World* world, const size_t id);
	}
}