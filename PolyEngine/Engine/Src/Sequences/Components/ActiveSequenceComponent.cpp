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
Poly::ActiveSequenceComponent::ActiveSequenceComponent(std::vector<String> sequencesNames)
	: Names(sequencesNames)
{
}

//------------------------------------------------------------------------------
void Poly::ActiveSequenceComponent::AddActiveSequence(String sequenceName)
{
	ASSERTE(std::find(Names.begin(), Names.end(), sequenceName) == Names.end(), "ActiveSequenceComponent already contains given sequenceName");
	Names.push_back(std::move(sequenceName));
}

//------------------------------------------------------------------------------
void Poly::ActiveSequenceComponent::AddActiveSequences(const std::vector<String>& sequencesNames)
{
	for (const auto& name : sequencesNames)
	{
		ASSERTE(std::find(Names.begin(), Names.end(), name) == Names.end(), "ActiveSequenceComponent already contains given sequenceName");
		Names.push_back(std::move(name));
	}	
}

//------------------------------------------------------------------------------
void ActiveSequenceComponent::RemoveActiveSequence(String sequenceName)
{
	Names.erase(std::remove(Names.begin(), Names.end(), sequenceName), Names.end());
}

//------------------------------------------------------------------------------
void Poly::ActiveSequenceComponent::RemoveActiveSequences(const std::vector<String>& sequencesNames)
{
	for (const auto& name : sequencesNames)
		Names.erase(std::remove(Names.begin(), Names.end(), name), Names.end());
}

//------------------------------------------------------------------------------
const std::vector<String>& ActiveSequenceComponent::GetActiveSequencesNames() const
{
	return Names;
}
