#include <EnginePCH.hpp>
#include <Sequences/Components/SequenceComponent.hpp>

using namespace Poly;

RTTI_DEFINE_COMPONENT(::Poly::SequenceComponent)

//------------------------------------------------------------------------------
SequenceComponent::SequenceComponent(std::map<String, Sequence>&& sequneces)
	: Sequences(sequneces)
{
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
