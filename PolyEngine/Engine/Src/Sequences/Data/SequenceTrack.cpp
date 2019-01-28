#include <EnginePCH.hpp>
#include <Sequences/Data/SequenceTrack.hpp>

using namespace Poly;

//------------------------------------------------------------------------------
void Poly::SequenceTrack::AppendAction(TimeDuration startTime, std::shared_ptr<IAction> action)
{
	ASSERTE(startTime >= TimeDuration(0), "Start time must be greater or equal 0.");

	if (Actions.size() > 0 && startTime < Actions[Actions.size() - 1].StartTime + Actions[Actions.size() - 1].Action->GetDuration())
		gConsole.LogError("Actions can't overlap each other.");
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
	ASSERTE(!Active, "Track must be inactive to begin.");

	EntityObj = entity;
	NextActionIndex = 0;
	ActiveAction = nullptr;
	Active = true;
	TrackElapsedTime = TimeDuration(0);
}

//------------------------------------------------------------------------------
void SequenceTrack::OnUpdate(const TimeDuration deltaTime)
{
	ASSERTE(Active, "Trackmust be active to update.");

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
	ASSERTE(Active, "Track must be active to abort.");

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
			timeToCompleteAction <= deltaTime)
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
