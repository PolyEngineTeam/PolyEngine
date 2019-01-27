#pragma once

#include <Defines.hpp>
#include <RTTI/RTTI.hpp>

#include <Sequences/Data/SequenceTrack.hpp>

namespace Poly
{
	class Entity;

	class ENGINE_DLLEXPORT Sequence
	{
	public:
		Sequence(String name);

		void AddTrack(SequenceTrack track);

		String GetName() const;
		bool IsActive();

		void OnBegin(Entity* entity);
		void OnUpdate(TimeDuration deltaTime);
		void OnAbort();

	private:
		String Name;
		std::vector<SequenceTrack> Tracks;

	}; // class Sequence
} // namespace Poly