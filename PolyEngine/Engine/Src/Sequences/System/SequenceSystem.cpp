#include <EnginePCH.hpp>
#include <Sequences/System/SequenceSystem.hpp>

// engine
#include <ECS/DeferredTaskSystem.hpp>
#include <Time/TimeSystem.hpp>

// this project
#include <Sequences/Components/ActiveSequenceComponent.hpp>
#include <Sequences/Components/SequenceComponent.hpp>
#include <Sequences/Data/Sequence.hpp>

using namespace Poly;

//------------------------------------------------------------------------------
void SequenceSystem::OnUpdate(Scene* scene)
{
	const TimeDuration deltaTime = TimeDuration((TimeSystem::GetTimerDeltaTime(scene, Poly::eEngineTimer::GAMEPLAY)));

	for (auto[activeSequenceComponent, sequenceComponent] : scene->IterateComponents<ActiveSequenceComponent, SequenceComponent>())
	{
		for (const auto& sequenceName : activeSequenceComponent->GetActiveSequencesNames())
			sequenceComponent->GetSequence(sequenceName).OnUpdate(deltaTime);
	}
}

//------------------------------------------------------------------------------
void SequenceSystem::StartSequence(Entity* entity, String name)
{
	if (auto* activeSequenceComponent = entity->GetComponent<ActiveSequenceComponent>())
		activeSequenceComponent->AddActiveSequence(std::move(name));
	else
		DeferredTaskSystem::AddComponent<ActiveSequenceComponent>(entity->GetEntityScene(), entity, std::move(name));

	entity->GetComponent<SequenceComponent>()->GetSequence(name).OnBegin(entity);
}
