#include <EnginePCH.hpp>
#include <Rendering/Animation/SkeletalAnimationComponent.hpp>
#include <Rendering/Animation/AnimationSystem.hpp>

RTTI_DEFINE_COMPONENT(::Poly::SkeletalAnimationComponent)

Poly::SkeletalAnimationComponent::~SkeletalAnimationComponent()
{
	Poly::AnimationSystem::OnComponentRemoved(this);
}
