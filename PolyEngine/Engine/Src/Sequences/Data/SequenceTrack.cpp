#include <EnginePCH.hpp>
#include <Sequences/Data/SequenceTrack.hpp>

using namespace Poly;

RTTI_DEFINE_TYPE(::Poly::SequenceTrack)

//------------------------------------------------------------------------------
SequenceTrack::SequenceTrack(std::vector<RegisteredAction>&& actions)
	: Actions(actions)
{
}

//------------------------------------------------------------------------------
bool SequenceTrack::IsActive()
{
	return ActiveAction != nullptr || NextActionIndex != Actions.size();
}

//------------------------------------------------------------------------------
void SequenceTrack::OnBegin(Entity* entity)
{
	EntityObj = entity;
	NextActionIndex = 0;
	ActiveAction = nullptr;
}

//------------------------------------------------------------------------------
void SequenceTrack::OnUpdate(const TimeDuration deltaTime)
{
	const TimePoint thisTimePoint = std::chrono::steady_clock::now();

	// if there is any action active
	if (ActiveAction)
	{
		ActiveAction->OnUpdate(deltaTime);

		// if action should finish now
		if (thisTimePoint - Actions[NextActionIndex - 1].StartTime >= ActiveAction->GetTotalTime())
			ActiveAction = nullptr;
	}

	// if there is no active action and next action should start now
	if (Actions[NextActionIndex].StartTime <= thisTimePoint)
	{
		ASSERTE(ActiveAction == nullptr, "Previous action hasn't finished yet.");
		ActiveAction = Actions[NextActionIndex].Action.get();
		ActiveAction->OnBegin(EntityObj);
		++NextActionIndex;
	}
}

//------------------------------------------------------------------------------
void SequenceTrack::OnAbort()
{
	EntityObj = nullptr;
	NextActionIndex = 0;

	if (ActiveAction)
	{
		ActiveAction->OnAbort();
		ActiveAction = nullptr;
	}
}
