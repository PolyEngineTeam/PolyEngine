#include <EnginePCH.hpp>
#include <Sequences/Components/StartSequenceComponent.hpp>

using namespace Poly;

RTTI_DEFINE_COMPONENT(::Poly::StartSequenceComponent)

//------------------------------------------------------------------------------
StartSequenceComponent::StartSequenceComponent(String sequenceName)
	: Names({ std::move(sequenceName) })
{
}

//------------------------------------------------------------------------------
StartSequenceComponent::StartSequenceComponent(Dynarray<String> sequencesNames)
	: Names(sequencesNames)
{
}

//------------------------------------------------------------------------------
void StartSequenceComponent::AddSequenceToStart(String sequenceName)
{
	ASSERTE(!Names.Contains(sequenceName), "ActiveSequenceComponent already contains given sequenceName");
	Names.PushBack(std::move(sequenceName));
}

//------------------------------------------------------------------------------
void StartSequenceComponent::AddSequencesToStart(const Dynarray<String>& sequencesNames)
{
	for (const auto& name : sequencesNames)
	{
		ASSERTE(!Names.Contains(name), "ActiveSequenceComponent already contains given sequenceName");
		Names.PushBack(std::move(name));
	}
}

//------------------------------------------------------------------------------
const Dynarray<String>& StartSequenceComponent::GetSequencesToStart() const
{
	return Names;
}
