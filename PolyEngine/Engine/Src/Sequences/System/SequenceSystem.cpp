#include <EnginePCH.hpp>
#include <Sequences/System/SequenceSystem.hpp>

// engine
#include <ECS/DeferredTaskSystem.hpp>
#include <Sequences/Components/ActiveSequenceComponent.hpp>

using namespace Poly;

//------------------------------------------------------------------------------
SequenceSystem::SequenceSystem()
{
}

//------------------------------------------------------------------------------
void SequenceSystem::OnUpdate(Scene* scene)
{
}

//------------------------------------------------------------------------------
void SequenceSystem::StartSequence(Entity* entity, String name)
{
	DeferredTaskSystem::AddComponent<ActiveSequenceComponent>(entity->GetEntityScene(), entity);
}
