#pragma once

#include <Defines.hpp>
#include <ECS/ComponentBase.hpp>
#include <Time/TimeSystem.hpp>

namespace Poly
{
	struct SkeletalAnimationParams final : public BaseObjectLiteralType<>
	{
		float Delay = 0.f;
		eEngineTimer Timer = eEngineTimer::GAMEPLAY;
		float PlaybackSpeed = 1.0f;
		bool Loop = false;
		float Weight = 1.0f;
		Optional<uint> LoopCount;
	};

	struct SkeletalAnimationState final : public BaseObjectLiteralType<>
	{
		SkeletalAnimationState() {};

		SkeletalAnimationState(const SkeletalAnimationParams& params)
			: Params(params)
		{
		}

		float DelayTime = 0.f;
		float Time = 0.f;
		uint LoopCount = 0;
		bool StopRequested = false;
		SkeletalAnimationParams Params;
	};

	class ENGINE_DLLEXPORT SkeletalAnimationComponent : public ComponentBase
	{
	public:
		RTTI_DECLARE_COMPONENT(::Poly::SkeletalAnimationComponent) { NO_RTTI_PROPERTY(); }

		~SkeletalAnimationComponent() override;

		std::map<String, SkeletalAnimationState> ActiveAnimations;
		std::map<String, Matrix> ModelFromBone;
		std::vector<SafePtr<Entity>> Bones;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, SkeletalAnimationComponent)
}