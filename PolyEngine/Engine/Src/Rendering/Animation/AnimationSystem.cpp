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

static const float MIN_TO_SEC = 60.f;

void AnimationSystem::OnUpdate(Scene* scene)
{
	for (auto&&[boneCmp] : scene->IterateComponents<BoneComponent>())
	{
		Vector parentPos = boneCmp->GetOwner()->GetParent()->GetTransform().GetGlobalTranslation();
		Vector myPos = boneCmp->GetOwner()->GetTransform().GetGlobalTranslation();
		//DebugDrawSystem::DrawLine(scene, myPos, parentPos, Color::GREEN);
		//DebugDrawSystem::DrawArrow(scene, myPos, boneCmp->GetOwner()->GetTransform().GetGlobalForward(), Color::RED);
	}

	for (auto&& [animCmp, meshCmp] : scene->IterateComponents<SkeletalAnimationComponent, MeshRenderingComponent>())
	{
		if (animCmp->CheckFlags(eComponentBaseFlags::NEWLY_CREATED))
			CreateBoneStructure(animCmp, meshCmp);

		std::map<String, std::vector<std::pair<Matrix, float>>> boneMatrices;
		std::vector<String> animsToremove;

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

			float dt = (float)TimeSystem::GetTimerDeltaTime(scene, animState.Params.Timer);

			dt *= animState.Params.PlaybackSpeed * MIN_TO_SEC;

			if (animState.DelayTime + dt < animState.Params.Delay)
			{
				animState.DelayTime += dt;
				continue;
			}
			else if (animState.DelayTime < animState.Params.Delay)
			{
				// update dt
				dt = animState.DelayTime + dt - animState.Params.Delay;
				animState.DelayTime = animState.Params.Delay;
			}

			// If animation finished
			animState.StopRequested = animState.StopRequested || (!animState.Params.Loop && animState.Time + dt > anim->Duration);
			// If looped animation finished
			animState.StopRequested = animState.StopRequested || (animState.Params.Loop && animState.Params.LoopCount.HasValue() 
				&& animState.LoopCount >= animState.Params.LoopCount.Value());

			if (animState.StopRequested && animState.Time + dt > anim->Duration)
			{
				// animation finished, set time to duration
				animState.Time = anim->Duration;
			}
			else
			{
				animState.Time = animState.Time + dt;
				if (animState.Time >= anim->Duration && !animState.StopRequested)
				{
					animState.Time = fmodf(animState.Time, anim->Duration);
					animState.LoopCount += 1;
				}
			}

			
			
			//ASSERTE(anim->TicksPerSecond == 0.f, "Unhandled tics per sec.");

			for (auto& channel : anim->channels)
			{
				const String& channelName = channel.first;
				auto lerpData = anim->GetLerpData(channelName, animState.Time);
				
				Vector pos = Vector::ZERO;
				Vector scale = Vector::ONE;
				Quaternion rot = Quaternion::IDENTITY;

				if (lerpData.pos[0].HasValue())
				{
					auto posKey = lerpData.pos[0].Value();
					pos = posKey.Value;
					if (lerpData.pos[1].HasValue())
					{
						pos = ::Poly::Lerp(pos, lerpData.pos[1].Value().Value, animState.Time - posKey.Time);
					}
				}
					
				if (lerpData.scale[0].HasValue())
				{
					auto scaleKey = lerpData.scale[0].Value();
					scale = scaleKey.Value;
					if (lerpData.scale[1].HasValue())
					{
						scale = ::Poly::Lerp(scale, lerpData.scale[1].Value().Value, animState.Time - scaleKey.Time);
					}
				}

				if (lerpData.rot[0].HasValue())
				{
					auto rotKey = lerpData.rot[0].Value();
					rot = rotKey.Value;
					if (lerpData.rot[1].HasValue())
					{
						const float t = std::min(animState.Time - rotKey.Time, 1.0f);
						rot = Quaternion::Slerp(rot, lerpData.rot[1].Value().Value, t);
					}
				}

				Matrix parentFromBone = Matrix::Compose(pos, rot, scale);
				boneMatrices[channelName].push_back({ parentFromBone, animState.Params.Weight });
			}

			if (animState.StopRequested && animState.Time >= anim->Duration)
				animsToremove.push_back(animName);
		}

		for (const String& animName : animsToremove)
		{
			animCmp->ActiveAnimations.erase(animName);
		}

		if (!boneMatrices.empty())
		{
			for (auto& bone : animCmp->Bones)
			{
				auto&& it = boneMatrices.find(bone->GetName());
				if(it != boneMatrices.end())
					bone->GetTransform().SetParentFromModel(Matrix::Blend(it->second));
			}
		}

		animCmp->ModelFromBone.clear();
		Matrix ModelFromWorld = animCmp->GetTransform().GetWorldFromModel().GetInversed();
		for (auto& bone : animCmp->Bones)
			animCmp->ModelFromBone[bone->GetName()] = ModelFromWorld * bone->GetTransform().GetWorldFromModel();

		for (auto& modelfrombone : animCmp->ModelFromBone)
		{
			auto worldFromModel = animCmp->GetTransform().GetWorldFromModel();
			auto worldfrombone = worldFromModel * modelfrombone.second;

			Vector globalbonePos = worldfrombone * Vector::ZERO;
			Vector a, b;
			Quaternion rot;
			worldfrombone.Decompose(a, rot, b);
			Vector fwd = rot * -Vector::UNIT_Z;

			DebugDrawSystem::DrawArrow(scene, globalbonePos, fwd, Color::RED);
		}
	}
}

void Poly::AnimationSystem::StartAnimation(SkeletalAnimationComponent* cmp, const String&  animationName, const SkeletalAnimationParams& params)
{
	auto&& it = cmp->ActiveAnimations.find(animationName);
	if (it != cmp->ActiveAnimations.end())
	{
		gConsole.LogWarning("Starting animation [{}] when it's already running.");
	} 

	cmp->ActiveAnimations[animationName] = SkeletalAnimationState(params);
}

void Poly::AnimationSystem::StopAnimation(SkeletalAnimationComponent* cmp, const String&  animationName, bool immediate)
{
	auto&& it = cmp->ActiveAnimations.find(animationName);
	if (it != cmp->ActiveAnimations.end())
	{
		gConsole.LogError("Stopping animation [{}] when it's not running.");
	}
	else
	{
		it->second.StopRequested = true;
	}
}

bool Poly::AnimationSystem::IsAnimationActive(SkeletalAnimationComponent * cmp, const String&  animationName)
{
	auto&& it = cmp->ActiveAnimations.find(animationName);
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
	const auto& bones = meshCmp->GetMesh()->GetBones();
	for (const auto& bone : bones)
	{
		Entity* ent = DeferredTaskSystem::SpawnEntityImmediate(scene);
		ent->SetName(bone.name);
		DeferredTaskSystem::AddComponentImmediate<BoneComponent>(scene, ent);
		animCmp->Bones.push_back(ent);
	}

	// Initialize bone hierarchy
	for (size_t i = 0; i < bones.GetSize(); ++i)
	{
		if (bones[i].parentBoneIdx.HasValue())
			animCmp->Bones[i]->SetParent(animCmp->Bones[bones[i].parentBoneIdx.Value()].Get());
		else
			animCmp->Bones[i]->SetParent(skeleton);
		animCmp->Bones[i]->GetTransform().SetParentFromModel(bones[i].prevBoneFromBone);
	}
}

void Poly::AnimationSystem::RemoveBoneStructure(SkeletalAnimationComponent * animCmp, MeshRenderingComponent * meshCmp)
{
}
