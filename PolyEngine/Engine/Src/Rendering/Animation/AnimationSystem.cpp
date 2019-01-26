#include <EnginePCH.hpp>
#include "AnimationSystem.hpp"
#include <ECS/Scene.hpp>

#include <Time/TimeSystem.hpp>
#include <Debugging/DebugDrawSystem.hpp>
#include <Rendering/MeshRenderingComponent.hpp>
#include <Rendering/Animation/BoneComponent.hpp>
#include <Rendering/Animation/SkeletalAnimationComponent.hpp>
#include <ECS/DeferredTaskSystem.hpp>


using namespace Poly;

void AnimationSystem::OnUpdate(Scene* scene)
{
	for (auto&[boneCmp] : scene->IterateComponents<BoneComponent>())
	{
		Vector parentPos = boneCmp->GetOwner()->GetParent()->GetTransform().GetGlobalTranslation();
		Vector myPos = boneCmp->GetOwner()->GetTransform().GetGlobalTranslation();
		DebugDrawSystem::DrawLine(scene, myPos, parentPos, Color::GREEN);
		DebugDrawSystem::DrawArrow(scene, myPos, boneCmp->GetOwner()->GetTransform().GetGlobalForward(), Color::RED);
	}

	for (auto& [animCmp, meshCmp] : scene->IterateComponents<SkeletalAnimationComponent, MeshRenderingComponent>())
	{
		if (animCmp->CheckFlags(eComponentBaseFlags::NEWLY_CREATED))
			CreateBoneStructure(animCmp, meshCmp);

		for (auto& animKV : animCmp->ActiveAnimations)
		{
			const String& animName = animKV.first;
			SkeletalAnimationState& animState = animKV.second;
			const MeshResource::Animation* anim = meshCmp->GetMesh()->GetAnimation(animName);

			if (!anim)
			{
				gConsole.LogError("Animation with name {} not found!", animName);
				continue;
			}

			const double dt = TimeSystem::GetTimerDeltaTime(scene, animState.Params.Timer);
		}
	}
}

void Poly::AnimationSystem::StartAnimation(SkeletalAnimationComponent* cmp, const String&  animationName, const SkeletalAnimationParams& params)
{
	auto& it = cmp->ActiveAnimations.find(animationName);
	if (it != cmp->ActiveAnimations.end())
	{
		gConsole.LogWarning("Starting animation [{}] when it's already running.");
	} 

	it->second = SkeletalAnimationState(params);
}

void Poly::AnimationSystem::StopAnimation(SkeletalAnimationComponent* cmp, const String&  animationName, bool immediate)
{
	auto& it = cmp->ActiveAnimations.find(animationName);
	if (it != cmp->ActiveAnimations.end())
	{
		gConsole.LogError("Stopping animation [{}] when it's not running.");
	}
	else
	{
		cmp->ActiveAnimations.erase(it);
	}
}

bool Poly::AnimationSystem::IsAnimationActive(SkeletalAnimationComponent * cmp, const String&  animationName)
{
	auto& it = cmp->ActiveAnimations.find(animationName);
	return it != cmp->ActiveAnimations.end();
}

void Poly::AnimationSystem::OnComponentRemoved(SkeletalAnimationComponent* cmp)
{
	RemoveBoneStructure(cmp, cmp->GetSibling< MeshRenderingComponent>());
	gConsole.LogDebug("SkeletalAnimationComponent removed successfully in AnimationSystem");
}

void Poly::AnimationSystem::CreateBoneStructure(SkeletalAnimationComponent* animCmp, MeshRenderingComponent* meshCmp)
{
	Entity* parent = animCmp->GetOwner();
	Scene* scene = parent->GetEntityScene();
	
	// Create skeleton
	Entity* skeleton = DeferredTaskSystem::SpawnEntityImmediate(scene);
	skeleton->SetParent(parent);
	skeleton->GetTransform().SetParentFromModel(meshCmp->GetMesh()->GetModelFromSkeletonRoot());
	skeleton->SetName("Mesh_Skeleton");

	// Create bones
	std::vector<Entity*> bonesEnt;
	const auto& bones = meshCmp->GetMesh()->GetBones();
	for (const auto& bone : bones)
	{
		Entity* ent = DeferredTaskSystem::SpawnEntityImmediate(scene);
		ent->SetName(bone.name);
		DeferredTaskSystem::AddComponentImmediate<BoneComponent>(scene, ent);
		bonesEnt.push_back(ent);
	}

	// Initialize bone hierarchy
	for (size_t i = 0; i < bones.GetSize(); ++i)
	{
		if (bones[i].parentBoneIdx.HasValue())
			bonesEnt[i]->SetParent(bonesEnt[bones[i].parentBoneIdx.Value()]);
		else
			bonesEnt[i]->SetParent(skeleton);
		bonesEnt[i]->GetTransform().SetParentFromModel(bones[i].prevBoneFromBone);
	}
}

void Poly::AnimationSystem::RemoveBoneStructure(SkeletalAnimationComponent * animCmp, MeshRenderingComponent * meshCmp)
{
}
