#include <EnginePCH.hpp>
#include <Sequences/Data/SequenceTrack.hpp>

using namespace Poly;

//------------------------------------------------------------------------------
void Poly::SequenceTrack::AppendAction(TimeDuration startTime, std::shared_ptr<IAction> action)
{
	ASSERTE(startTime >= TimeDuration(0), "Start time must be greater or equal 0.");

	if (Actions.size() > 0 && startTime < Actions[Actions.size() - 1].StartTime)
		gConsole.LogError("Can't add action starting earlier than previous actions in this track.");
	else
		Actions.push_back({ startTime, std::move(action) });
}

//------------------------------------------------------------------------------
bool SequenceTrack::IsActive()
{
	return Active;
}

//------------------------------------------------------------------------------
void SequenceTrack::OnBegin(Entity* entity)
{
	ASSERTE(!Active, "To update track you must call OnAbort first or this must be the first call for this instance.");

	EntityObj = entity;
	NextActionIndex = 0;
	ActiveAction = nullptr;
	Active = true;
	TrackStartTime = std::chrono::steady_clock::now();
}

//------------------------------------------------------------------------------
void SequenceTrack::OnUpdate(const TimeDuration deltaTime)
{
	ASSERTE(Active, "To update track you must call OnBegin first.");

	const TimeDuration trackDuration = std::chrono::steady_clock::now() - TrackStartTime;

	TryUpdateActiveAction(trackDuration, deltaTime);
	TryStartNextAction(trackDuration, deltaTime);
	TryFinishTrack();
}

//------------------------------------------------------------------------------
void SequenceTrack::OnAbort()
{
	ASSERTE(Active, "To abort track you must call OnBegin first.");

	Active = false;
}

//------------------------------------------------------------------------------
void SequenceTrack::TryUpdateActiveAction(TimeDuration trackDuration, TimeDuration deltaTime)
{
	if (ActiveAction)
	{
		ActiveAction->OnUpdate(deltaTime);
		TryFinishActiveAction(trackDuration, deltaTime);
	}
}

//------------------------------------------------------------------------------
void SequenceTrack::TryFinishActiveAction(TimeDuration trackDuration, TimeDuration deltaTime)
{
	if (trackDuration - Actions[NextActionIndex - 1].StartTime >= ActiveAction->GetDuration())
	{
		ActiveAction->OnFinish();
		ActiveAction = nullptr;
	}
}

//------------------------------------------------------------------------------
void SequenceTrack::TryStartNextAction(TimeDuration trackDuration, TimeDuration deltaTime)
{
	if (NextActionIndex < Actions.size() && Actions[NextActionIndex].StartTime <= trackDuration)
	{
		ASSERTE(ActiveAction == nullptr, "Previous action hasn't finished yet.");
		ActiveAction = Actions[NextActionIndex].Action.get();
		ActiveAction->OnBegin(EntityObj);
		++NextActionIndex;
	}
}

//------------------------------------------------------------------------------
void SequenceTrack::TryFinishTrack()
{
	if (NextActionIndex == Actions.size() && ActiveAction == nullptr)
		Active = false;
}
