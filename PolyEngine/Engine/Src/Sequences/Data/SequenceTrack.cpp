#include <EnginePCH.hpp>
#include <Sequences/Data/SequenceTrack.hpp>

using namespace Poly;

//------------------------------------------------------------------------------
void Poly::SequenceTrack::AddAction(RegisteredAction action)
{
	Actions.push_back(std::move(action));
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
	TrackStartTime = std::chrono::steady_clock::now();
}

//------------------------------------------------------------------------------
void SequenceTrack::OnUpdate(const TimeDuration deltaTime)
{
	const TimePoint thisTimePoint = std::chrono::steady_clock::now();
	const TimeDuration trackDuration = thisTimePoint - TrackStartTime;

	// if there is any action active
	if (ActiveAction)
	{
		ActiveAction->OnUpdate(deltaTime);

		// if action should finish now
		if (trackDuration - Actions[NextActionIndex - 1].StartTime >= ActiveAction->GetTotalTime())
			ActiveAction = nullptr;
	}

	// if there is no active action and next action should start now
	if (Actions[NextActionIndex].StartTime <= trackDuration)
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
