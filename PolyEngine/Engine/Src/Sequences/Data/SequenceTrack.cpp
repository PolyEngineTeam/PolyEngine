#include <EnginePCH.hpp>
#include <Sequences/Data/SequenceTrack.hpp>

using namespace Poly;

RTTI_DEFINE_TYPE(::Poly::SequenceTrack)

//------------------------------------------------------------------------------
SequenceTrack::SequenceTrack(std::vector<RegisteredAction>&& actions)
	: Actions(actions), NextAction(Actions.begin())
{
}

//------------------------------------------------------------------------------
bool SequenceTrack::IsActive()
{
	return ActiveActions.size() > 0 && NextAction == Actions.end();
}

//------------------------------------------------------------------------------
void SequenceTrack::OnBegin(Entity* entity)
{
	Time = 0;
	EntityObj = entity;
	NextAction = Actions.begin();
}

//------------------------------------------------------------------------------
void SequenceTrack::OnUpdate(const float deltaTime)
{
	TryActivateNextAction(deltaTime);
	UpdateActiveActions(deltaTime);
}

//------------------------------------------------------------------------------
void SequenceTrack::OnAbort()
{
	// abort all active actions
	for (auto* action : ActiveActions)
		action->OnAbort();
}

//------------------------------------------------------------------------------
void SequenceTrack::UpdateActiveActions(const float deltaTime)
{
	// update actions
	size_t idx = 0;
	std::vector<size_t> indcesToRemove;
	for (auto* action : ActiveActions)
	{
		action->OnUpdate(deltaTime);

		if (!action->IsActive())
			indcesToRemove.insert(indcesToRemove.begin(), idx);
	}

	// remove inactive actions
	for (auto idx : indcesToRemove)
		ActiveActions.erase(ActiveActions.begin() + idx);
}

//------------------------------------------------------------------------------
void SequenceTrack::TryActivateNextAction(const float deltaTime)
{
	// add next actions to active actions if necessary
	while (NextAction != Actions.end() && NextAction->StartTime <= Time)
	{
		NextAction->Action->OnBegin(EntityObj);
		ActiveActions.insert(ActiveActions.end(), NextAction->Action.get());
	}
}
