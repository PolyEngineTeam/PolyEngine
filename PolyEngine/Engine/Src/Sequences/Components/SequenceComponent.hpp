#pragma once

#include <Defines.hpp>
#include <ECS/ComponentBase.hpp>

#include <Sequences/Data/Sequence.hpp>

namespace Poly
{
	//------------------------------------------------------------------------------
	class ENGINE_DLLEXPORT SequenceComponent : public ComponentBase
	{
		RTTI_DECLARE_COMPONENT(::Poly::SequenceComponent)
		{ 
			NO_RTTI_PROPERTY(); 
		}

	public:
		void AddSequence(Sequence sequenceToAdd);

		const std::map<String, Sequence>& GetSequences() const;
		Sequence& GetSequence(const String& sequenceName);

	private:
		std::map<String, Sequence> Sequences;

	};

	REGISTER_COMPONENT(ComponentsIDGroup, SequenceComponent)
} // namespace Poly