#include <EnginePCH.hpp>
#include <Sequences/System/SequenceSystem.hpp>

// engine
#include <ECS/DeferredTaskSystem.hpp>
#include <Time/TimeSystem.hpp>

// this project
#include <Sequences/Components/ActiveSequenceComponent.hpp>
#include <Sequences/Components/SequenceComponent.hpp>
#include <Sequences/Components/StartSequenceComponent.hpp>
#include <Sequences/Data/Sequence.hpp>

using namespace Poly;

//------------------------------------------------------------------------------
void SequenceSystem::OnUpdate(Scene* scene)
{
	const TimeDuration deltaTime = TimeDuration((TimeSystem::GetTimerDeltaTime(scene, Poly::eEngineTimer::GAMEPLAY)));

	// startrequested sequences
	for (auto&&[startSequenceComponent, sequenceComponent] : scene->IterateComponents<StartSequenceComponent, SequenceComponent>())
	{
		Entity* entity = startSequenceComponent->GetOwner();

		// add sequences to ActiveSequenceComponent
		if (auto* activeSequenceComponent = startSequenceComponent->GetSibling<ActiveSequenceComponent>())
			activeSequenceComponent->AddActiveSequences(startSequenceComponent->GetSequencesToStart());
		else
		{
			DeferredTaskSystem::AddComponent<ActiveSequenceComponent>(scene, entity, std::move(startSequenceComponent->GetSequencesToStart()));
		}

		// call OnBegin for all just started sequences
		sequenceComponent = entity->GetComponent<SequenceComponent>();
		for (auto& sequenceName : startSequenceComponent->GetSequencesToStart())
			sequenceComponent->GetSequence(sequenceName).OnBegin(entity);

		// remove unnecessary component
		DeferredTaskSystem::RemoveComponent<StartSequenceComponent>(scene, entity);
	}

	std::vector<Entity*> entitiesWithFinishedSequences;

	// update all active sequences
	for (auto&&[activeSequenceComponent, sequenceComponent] : scene->IterateComponents<ActiveSequenceComponent, SequenceComponent>())
	{
		std::vector<String> sequencesToStop;

		for (const auto& sequenceName : activeSequenceComponent->GetActiveSequencesNames())
		{
			auto& sequence = sequenceComponent->GetSequence(sequenceName);
			sequence.OnUpdate(deltaTime);

			if (!sequence.IsActive())
				sequencesToStop.push_back(sequenceName);
		}

		activeSequenceComponent->RemoveActiveSequences(sequencesToStop);

		if (activeSequenceComponent->GetActiveSequencesNames().size() == 0)
			entitiesWithFinishedSequences.push_back(activeSequenceComponent->GetOwner());
	}

	// remove unnecessary components
	for (auto* entity : entitiesWithFinishedSequences)
		DeferredTaskSystem::RemoveComponent<ActiveSequenceComponent>(scene, entity);
}