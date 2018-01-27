#include "EnginePCH.hpp"
#include "ParticleEmitter.hpp"

using namespace Poly;

Poly::ParticleEmitter::ParticleEmitter(int size)
{
	ParticleProxy = gEngine->GetRenderingDevice()->CreateParticle();
	Burst(size);
}

void ParticleEmitter::Burst(int size)
{
	InstancesTransform.Resize(16 * size);
	int index = 0;
	for (int i = 0; i < InstancesTransform.GetSize() / 16; ++i)
	{
		gConsole.LogInfo("ParticleEmitter::Burst p#: {}", index);
		// identity
		InstancesTransform[index + 0] = 1.0f;
		InstancesTransform[index + 5] = 1.0f;
		InstancesTransform[index + 10] = 1.0f;
		InstancesTransform[index + 15] = 1.0f;
		// translation
		InstancesTransform[index + 12] = 5.0f * Random(-1.0, 1.0);
		InstancesTransform[index + 13] = 5.0f * Random(-1.0, 1.0);
		InstancesTransform[index + 14] = 5.0f * Random(-1.0, 1.0);
		index += 16;
	}

	UpdateDeviceProxy();
}

void ParticleEmitter::Update()
{
	gConsole.LogInfo("ParticleEmitter::Update");

	int index = 0;
	for (int i = 0; i < InstancesTransform.GetSize() / 16; ++i)
	{
		// identity
		InstancesTransform[index + 0] = 1.0f;
		InstancesTransform[index + 5] = 1.0f;
		InstancesTransform[index + 10] = 1.0f;
		InstancesTransform[index + 15] = 1.0f;
		// translation
		InstancesTransform[index + 12] = 0.99f * InstancesTransform[index + 12];
		InstancesTransform[index + 13] = 0.99f * InstancesTransform[index + 13];
		InstancesTransform[index + 14] = 0.99f * InstancesTransform[index + 14];
		index += 16;
	}

	UpdateDeviceProxy();
}

void ParticleEmitter::UpdateDeviceProxy()
{
	ParticleProxy->SetContent(*this);
}

float ParticleEmitter::Random() const
{
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

float ParticleEmitter::Random(float min, float max) const
{
	float rnd = Random();
	return Lerp(min, max, rnd);
}
