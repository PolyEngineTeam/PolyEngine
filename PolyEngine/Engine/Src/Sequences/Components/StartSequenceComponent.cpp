#include <EnginePCH.hpp>
#include <Sequences/Components/StartSequenceComponent.hpp>

using namespace Poly;

RTTI_DEFINE_COMPONENT(::Poly::StartSequenceComponent)

//------------------------------------------------------------------------------
void StartSequenceComponent::AddSequenceToStart(String sequenceName)
{
	ASSERTE(std::find(Names.begin(), Names.end(), sequenceName) == Names.end(), "ActiveSequenceComponent already contains given sequenceName");
	Names.push_back(std::move(sequenceName));
}

//------------------------------------------------------------------------------
void StartSequenceComponent::AddSequencesToStart(const std::vector<String>& sequencesNames)
{
	for (const auto& name : sequencesNames)
	{
		ASSERTE(std::find(Names.begin(), Names.end(), name) == Names.end(), "ActiveSequenceComponent already contains given sequenceName");
		Names.push_back(std::move(name));
	}
}

//------------------------------------------------------------------------------
const std::vector<String>& StartSequenceComponent::GetSequencesToStart() const
{
	return Names;
}
