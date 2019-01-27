#include <EnginePCH.hpp>
#include <Sequences/Components/SequenceComponent.hpp>

using namespace Poly;

RTTI_DEFINE_COMPONENT(::Poly::SequenceComponent)

//------------------------------------------------------------------------------
void SequenceComponent::AddSequence(Sequence sequenceToAdd)
{
	Sequences.insert({ sequenceToAdd.GetName(), std::move(sequenceToAdd) });
}

//------------------------------------------------------------------------------
const std::map<String, Sequence>& Poly::SequenceComponent::GetSequences() const
{
	return Sequences;
}

//------------------------------------------------------------------------------
Sequence& Poly::SequenceComponent::GetSequence(const String& sequenceName)
{
	return Sequences.find(sequenceName)->second;
}
