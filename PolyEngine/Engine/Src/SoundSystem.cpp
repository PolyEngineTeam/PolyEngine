#include "EnginePCH.hpp"

using namespace Poly;

//---------------------------------------------------------------------------------------------------
void SoundSystem::SoundPhase(World* world)
{
	gEngine->GetSoundDevice()->RenderWorld(world);
}
