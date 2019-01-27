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
	TrackElapsedTime = TimeDuration(0);
}

//------------------------------------------------------------------------------
void SequenceTrack::OnUpdate(const TimeDuration deltaTime)
{
	ASSERTE(Active, "To update track you must call OnBegin first.");

	TrackElapsedTime += deltaTime;

	// if there is no active action
	if (!ActiveAction)
		TryStartNextAction();
	
	// if there is active action 
	TimeDuration restOfDeltaTime = TryUpdateActiveAction(deltaTime);

	while (TryStartNextAction())
	{
		restOfDeltaTime = TryUpdateActiveAction(restOfDeltaTime);
	}

	TryFinishTrack();
}

//------------------------------------------------------------------------------
void SequenceTrack::OnAbort()
{
	ASSERTE(Active, "To abort track you must call OnBegin first.");

	if (ActiveAction)
		ActiveAction->OnAbort();

	Active = false;
}

//------------------------------------------------------------------------------
TimeDuration SequenceTrack::TryUpdateActiveAction(TimeDuration deltaTime)
{
	TimeDuration restOfDeltaTime = TimeDuration(0);

	if (ActiveAction)
	{
		TimeDuration actionElapsedTime = TrackElapsedTime - deltaTime - Actions[NextActionIndex - 1].StartTime;

		// if starts during this frame
		if (actionElapsedTime < TimeDuration(0))
		{
			actionElapsedTime += deltaTime;
			ActiveAction->OnUpdate(actionElapsedTime);
		}
		// if ends in this frame
		else if (const TimeDuration timeToCompleteAction = ActiveAction->GetDuration() - actionElapsedTime;
			timeToCompleteAction < deltaTime)
		{
			ActiveAction->OnUpdate(timeToCompleteAction);

			restOfDeltaTime = deltaTime - timeToCompleteAction;

			ActiveAction->OnFinish();
			ActiveAction = nullptr;
		}
		else
			ActiveAction->OnUpdate(deltaTime);

	}

	return restOfDeltaTime;
}

//------------------------------------------------------------------------------
bool SequenceTrack::TryStartNextAction()
{
	bool result = false;

	if (NextActionIndex < Actions.size() && Actions[NextActionIndex].StartTime <= TrackElapsedTime)
	{
		ASSERTE(ActiveAction == nullptr, "Previous action hasn't finished yet.");
		ActiveAction = Actions[NextActionIndex].Action.get();
		ActiveAction->OnBegin(EntityObj);
		++NextActionIndex;
		result = true;
	}

	return result;
}

//------------------------------------------------------------------------------
void SequenceTrack::TryFinishTrack()
{
	if (NextActionIndex == Actions.size() && ActiveAction == nullptr)
		Active = false;
}
