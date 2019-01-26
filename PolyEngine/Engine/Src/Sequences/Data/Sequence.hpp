#pragma once

#include <Defines.hpp>
#include <RTTI/RTTI.hpp>

#include <Sequences/Data/SequenceTrack.hpp>

namespace Poly
{
	class Entity;

	class Sequence : public RTTIBase
	{
		RTTI_DECLARE_TYPE_DERIVED(::Poly::Sequence, ::Poly::RTTIBase)
		{
			NO_RTTI_PROPERTY();
		}

	public:
		Sequence(String name, std::vector<SequenceTrack>&& tracks);

		String GetName() const;

		void OnBegin(Entity* entity);
		void OnUpdate(float deltaTime);
		void OnAbort();

	private:
		std::vector<SequenceTrack> Tracks;
		String Name;

	}; // class Sequence
} // namespace Poly