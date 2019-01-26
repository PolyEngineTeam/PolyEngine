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
		SequenceComponent(std::map<String, Sequence>&& sequneces);

		const std::map<String, Sequence>& GetSequences() const;
		Sequence& GetSequence(const String& sequenceName);

	private:
		std::map<String, Sequence> Sequences;

	}; // class SequenceComponent
} // namespace Poly