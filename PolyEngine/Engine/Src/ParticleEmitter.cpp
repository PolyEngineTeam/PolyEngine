#include "EnginePCH.hpp"
#include "ParticleEmitter.hpp"

using namespace Poly;

void ParticleEmitter::Burst(int quota)
{
	InstancesTransform.Resize(quota);
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
		InstancesTransform[index + 12] = 5.0f * Random(-1.0, 1.0);
		InstancesTransform[index + 13] = 5.0f * Random(-1.0, 1.0);
		InstancesTransform[index + 14] = 5.0f * Random(-1.0, 1.0);
		index += 16;
	}
}

void ParticleEmitter::UpdateDeviceProxy()
{
	ParticleProxy = gEngine->GetRenderingDevice()->CreateParticle();
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
