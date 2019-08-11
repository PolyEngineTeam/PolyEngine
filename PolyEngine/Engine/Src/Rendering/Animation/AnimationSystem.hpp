#pragma once

#include <pe/Defines.hpp>
#include <ECS/ISystem.hpp>

namespace Poly
{
	class MeshRenderingComponent;
	class SkeletalAnimationComponent;
	struct SkeletalAnimationParams;

	class ENGINE_DLLEXPORT AnimationSystem : public ISystem
	{
	public:
		virtual void OnUpdate(Scene* scene) override;

		static void StartAnimation(SkeletalAnimationComponent* cmp, const String& animationName, const SkeletalAnimationParams& params);
		static void StopAnimation(SkeletalAnimationComponent* cmp, const String&  animationName, bool immediate = false);
		static bool IsAnimationActive(SkeletalAnimationComponent* cmp, const String&  animationName);
		static void OnComponentRemoved(SkeletalAnimationComponent* cmp);

	private:
		static void CreateBoneStructure(SkeletalAnimationComponent* animCmp, MeshRenderingComponent* meshCmp);
		static void RemoveBoneStructure(SkeletalAnimationComponent* animCmp, MeshRenderingComponent* meshCmp);
	};
}