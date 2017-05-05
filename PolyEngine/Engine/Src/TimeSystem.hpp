#pragma once


namespace Poly
{
	class World;

	namespace TimeSystem
	{
		void TimeUpdatePhase(World* world);

		void RegisterTimer(World* world, size_t id, bool isPausable);

		float GetDeltaTime(World* world);

		float GetTimerTime(World* world, size_t id);
	}
}