#include "EnginePCH.hpp"
#include "ParticleEmitter.hpp"

#include <algorithm>

using namespace Poly;

ParticleEmitter::ParticleEmitter(Settings settings)
	: settings(settings), ParticlesPool(1000)
{
	ParticleProxy = gEngine->GetRenderingDevice()->CreateParticle();
	Emit(settings.InitialSize);
}

void ParticleEmitter::Emit(size_t size)
{
	size_t sizeLeft = ParticlesPool.GetFreeBlockCount();
	if (size > sizeLeft)
	{
		gConsole.LogInfo("ParticleEmitter::Emit not enough memory in room (1000)");
	}

	size_t amount = Clamp(size, (size_t)0, sizeLeft);

	// gConsole.LogInfo("ParticleEmitter::Emit emitLen: {}", amount);

	while (amount > 0)
	{
		Particle* p = ParticlesPool.Alloc();
		::new(p) Particle();

		settings.ParticleInitFunc(p);

		--amount;
	}

	RecreateBufferForProxy();

	UpdateDeviceProxy();
}

void ParticleEmitter::Update(World* world)
{
	// gConsole.LogInfo("ParticleEmitter::Update");

	float deltaTime = (float)(TimeSystem::GetTimerDeltaTime(world, Poly::eEngineTimer::GAMEPLAY));

	if (IsBurstEnabled)
	{
		if (NextBurstTime < 0.0f)
		{
			NextBurstTime = Random(settings.BurstTimeMin, settings.BurstTimeMax);
			Emit((int)Random(settings.BurstSizeMin, settings.BurstSizeMax));
		}
		else
		{
			NextBurstTime -= deltaTime;
		}
	}

	Dynarray<Particle*> ParticleToDelete;

	for (Particle& p : ParticlesPool)
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


	for (Particle* p : ParticleToDelete)
	{
		p->~Particle();
		ParticlesPool.Free(p);
	}

	for (Particle& p : ParticlesPool)
	{
		settings.ParticleUpdateFunc(&p);
	}

	RecreateBufferForProxy();

	UpdateDeviceProxy();
}

void ParticleEmitter::RecreateBufferForProxy()
{
	InstancesTransform.Clear();
	InstancesTransform.Resize(16 * ParticlesPool.GetSize());

	for (int i = 0; i < InstancesTransform.GetSize(); ++i)
	{
		InstancesTransform[i] = 0.0f;
	}

	int transIndx = 0;
	for (Particle& p : ParticlesPool)
	{
		// Scale
		InstancesTransform[transIndx + 0] = p.Scale.X;
		InstancesTransform[transIndx + 5] = p.Scale.Y;
		InstancesTransform[transIndx + 10] = p.Scale.Z;
		InstancesTransform[transIndx + 15] = 1.0f;
		// translation
		InstancesTransform[transIndx + 12] = p.Position.X;
		InstancesTransform[transIndx + 13] = p.Position.Y;
		InstancesTransform[transIndx + 14] = p.Position.Z;
		transIndx += 16;
	}
}

void ParticleEmitter::UpdateDeviceProxy()
{
	ParticleProxy->SetContent(*this);
}

float ParticleEmitter::Random()
{
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

float ParticleEmitter::Random(float min, float max)
{
	float rnd = Random();
	return Lerp(min, max, rnd);
}