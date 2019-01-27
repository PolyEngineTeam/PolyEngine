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
	return ActiveAction != nullptr || NextActionIndex != Actions.size();
}

//------------------------------------------------------------------------------
void SequenceTrack::OnBegin(Entity* entity)
{
	ASSERTE(EntityObj == nullptr, "To update track you must call OnAbort first or this must be the first call for this instance.");

	EntityObj = entity;
	NextActionIndex = 0;
	ActiveAction = nullptr;
	TrackStartTime = std::chrono::steady_clock::now();
}

//------------------------------------------------------------------------------
void SequenceTrack::OnUpdate(const TimeDuration deltaTime)
{
	ASSERTE(EntityObj != nullptr, "To update track you must call OnBegin first.");

	const TimeDuration trackDuration = std::chrono::steady_clock::now() - TrackStartTime;

	// if there is any action active
	if (ActiveAction)
	{
		ActiveAction->OnUpdate(deltaTime);

		// if action should finish now
		if (trackDuration - Actions[NextActionIndex - 1].StartTime >= ActiveAction->GetDuration())
			ActiveAction = nullptr;
	}

	// if there is no active action and next action should start now
	if (NextActionIndex < Actions.size() && Actions[NextActionIndex].StartTime <= trackDuration)
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
	ASSERTE(EntityObj != nullptr, "To abort track you must call OnBegin first.");

	EntityObj = nullptr;
	NextActionIndex = 0;

	if (ActiveAction)
	{
		ActiveAction->OnAbort();
		ActiveAction = nullptr;
	}
}
