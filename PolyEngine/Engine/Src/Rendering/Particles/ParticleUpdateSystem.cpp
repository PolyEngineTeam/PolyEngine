#include <EnginePCH.hpp>

#include <Rendering/Particles/ParticleUpdateSystem.hpp>
#include <Rendering/Particles/ParticleComponent.hpp>
#include <ECS/Scene.hpp>
#include <Time/TimeSystem.hpp>

using namespace Poly;

void ParticleUpdateSystem::ParticleUpdatePhase(Scene* world)
{
	// Render meshes
	for (auto compTuple : world->IterateComponents<ParticleComponent>())
	{
		ParticleComponent* particle = std::get<ParticleComponent*>(compTuple);
		ParticleEmitter* emitter = particle->Emitter;
		EmitterEmit(world, emitter, particle);
		EmitterUpdate(world, emitter);
		EmitterRecreateBuffer(world, emitter);
	}
}

void ParticleUpdateSystem::EmitterEmit(Scene* world, ParticleEmitter* emitter, ParticleComponent* particleCmp)
{
	size_t size = emitter->ToEmit;
	emitter->ToEmit = 0;
	size_t sizeLeft = emitter->ParticlesPool.GetFreeBlockCount();
	if (size > sizeLeft)
	{
		gConsole.LogInfo("ParticleEmitter::Emit not enough memory in room (1000)");
	}

	size_t amount = Clamp(size, (size_t)0, sizeLeft);

	// gConsole.LogInfo("ParticleEmitter::Emit emitLen: {}", amount);
	Vector PositionInModel = emitter->GetSettings().SimulationSpace == ParticleEmitter::eSimulationSpace::WORLD_SPACE
		? particleCmp->GetTransform().GetGlobalTranslation()
		: Vector::ZERO;

	while (amount > 0)
	{
		ParticleEmitter::Particle* p = emitter->ParticlesPool.Alloc();
		::new(p) ParticleEmitter::Particle();
		
		p->Position = PositionInModel;
		p->Scale = Vector::ONE;
		p->Velocity = Vector::ZERO;
		p->Acceleration = Vector::ZERO;
		p->Age = 0.0f;
		p->LifeTime = 1.0f;

		emitter->settings.ParticleInitFunc(p);

		--amount;
	}
}

void ParticleUpdateSystem::EmitterUpdate(Scene* world, ParticleEmitter* emitter)
{
	// gConsole.LogInfo("ParticleEmitter::Update {}/{}", emitter->ParticlesPool.GetSize(), emitter->settings.MaxSize);

	float deltaTime = (float)(TimeSystem::GetTimerDeltaTime(world, Poly::eEngineTimer::GAMEPLAY));

	if (emitter->IsBurstEnabled)
	{
		if (emitter->NextBurstTime < 0.0f)
		{
			emitter->NextBurstTime = RandomRange(emitter->settings.BurstTimeMin, emitter->settings.BurstTimeMax);
			emitter->Emit((size_t)RandomRange(emitter->settings.BurstSizeMin, emitter->settings.BurstSizeMax));
		}
		else
		{
			emitter->NextBurstTime -= deltaTime;
		}
	}

	Dynarray<ParticleEmitter::Particle*> ParticleToDelete;

	for (ParticleEmitter::Particle& p : emitter->ParticlesPool)
	{
		p.Age += deltaTime;
		if (p.Age > p.LifeTime)
		{
			ParticleToDelete.PushBack(&p);
		}
	}

	// if (ParticleToDelete.GetSize() > 0)
	// {
	// 	gConsole.LogInfo("ParticleEmitter::Update toDeleteLen: {}", ParticleToDelete.GetSize());
	// }

	for (ParticleEmitter::Particle* p : ParticleToDelete)
	{
		p->~Particle();
		emitter->ParticlesPool.Free(p);
	}

	for (ParticleEmitter::Particle& p : emitter->ParticlesPool)
	{
		p.Velocity += p.Acceleration;
		p.Position += p.Velocity;

		emitter->settings.ParticleUpdateFunc(&p);
	}
}

void ParticleUpdateSystem::EmitterRecreateBuffer(Scene* world, ParticleEmitter* emitter)
{
	emitter->ParticleProxy->SetContent(*emitter);
}
