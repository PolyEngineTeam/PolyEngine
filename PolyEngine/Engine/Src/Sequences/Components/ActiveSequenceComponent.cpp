#include <EnginePCH.hpp>
#include <Sequences/Components/ActiveSequenceComponent.hpp>

using namespace Poly;

RTTI_DEFINE_COMPONENT(::Poly::ActiveSequenceComponent)

//------------------------------------------------------------------------------
Poly::ActiveSequenceComponent::ActiveSequenceComponent(String sequenceName)
	: Names({ std::move(sequenceName) })
{
}

//------------------------------------------------------------------------------
Poly::ActiveSequenceComponent::ActiveSequenceComponent(Dynarray<String> sequencesNames)
	: Names(sequencesNames)
{
}

//------------------------------------------------------------------------------
void Poly::ActiveSequenceComponent::AddActiveSequence(String sequenceName)
{
	ASSERTE(!Names.Contains(sequenceName), "ActiveSequenceComponent already contains given sequenceName");
	Names.PushBack(std::move(sequenceName));
}

//------------------------------------------------------------------------------
void Poly::ActiveSequenceComponent::AddActiveSequence(const Dynarray<String>& sequencesNames)
{
	for (const auto& name : sequencesNames)
	{
		ASSERTE(!Names.Contains(name), "ActiveSequenceComponent already contains given sequenceName");
		Names.PushBack(std::move(name));
	}	
}

//------------------------------------------------------------------------------
const Dynarray<String>& ActiveSequenceComponent::GetActiveSequencesNames() const
{
	return Names;
}
