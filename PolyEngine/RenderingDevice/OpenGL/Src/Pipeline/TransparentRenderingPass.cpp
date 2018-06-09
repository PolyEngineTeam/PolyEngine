#include "Pipeline/TransparentRenderingPass.hpp"

#include "GLRenderingDevice.hpp"
#include "Proxy/GLMeshDeviceProxy.hpp"
#include "Proxy/GLTextureDeviceProxy.hpp"

#include <Math/AABox.hpp>
#include <ECS/World.hpp>
#include <Time/TimeSystem.hpp>
#include <Rendering/Camera/CameraComponent.hpp>
#include <Movement/MovementSystem.hpp>
#include <Rendering/MeshRenderingComponent.hpp>
#include <Rendering/PostprocessSettingsComponent.hpp>

using namespace Poly;

TransparentRenderingPass::TransparentRenderingPass(const GLRenderingDevice* rdi)
	: RenderingPassBase(rdi, "Shaders/transparentVert.shader", "Shaders/transparentFrag.shader")
{
}

void TransparentRenderingPass::OnRun(World* world, const CameraComponent* camera, const AARect& /*rect*/, ePassType /*passType = ePassType::GLOBAL*/ )
{
	float Time = (float)TimeSystem::GetTimerElapsedTime(world, eEngineTimer::GAMEPLAY);
	//const TransformComponent* CameraTransform = camera->GetSibling<TransformComponent>();

	GetProgram().BindProgram();

	GetProgram().SetUniform("uTime", Time);
	// GetProgram().SetUniform("uResolution", ResolutionX, ResolutionY);
	//gConsole.LogInfo("void TransparentRenderingPass::OnRun: UseCashetes: {}", PostprocessSettings->UseCashetes);

	const float cameraHeight = 16.f + 1.f;
	float verticalSpan = cameraHeight / 2.0f;
	float horizontalSpan = (cameraHeight * camera->GetAspect()) / 2.0f;
	Vector cameraPos = camera->GetTransform().GetGlobalTranslation();

	// Render meshes
	for (auto componentsTuple : world->IterateComponents<MeshRenderingComponent>())
	{
		const MeshRenderingComponent* meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);
		const EntityTransform& trans = meshCmp->GetTransform();

		if (!meshCmp->IsTransparent())
			continue;

		Vector objPos = trans.GetGlobalTranslation();

		bool shouldCull = objPos.Y > cameraPos.Y + verticalSpan;
		shouldCull = shouldCull || objPos.Y < cameraPos.Y - verticalSpan;
		shouldCull = shouldCull || objPos.X > cameraPos.X + horizontalSpan;
		shouldCull = shouldCull || objPos.X < cameraPos.X - horizontalSpan;
		if (shouldCull)
			continue;

		Matrix objScale;
		objScale.SetScale(Vector(1.0f, 1.0f, 1.0f));

		Matrix objTransform; // = transCmp->GetGlobalTransformationMatrix();
		objTransform.SetTranslation(trans.GetGlobalTranslation() + Vector(0.0f, 0.0f, 0.5f));

		Matrix screenTransform = camera->GetClipFromWorld() * objTransform * objScale;
		GetProgram().SetUniform("uTransform", objTransform * objScale);
		GetProgram().SetUniform("uMVPTransform", screenTransform);
		
		int i = 0;
		for (const MeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
		{
			GetProgram().SetUniform("uBaseColor", meshCmp->GetMaterial(i).DiffuseColor);
			UNUSED(subMesh);
			//const GLMeshDeviceProxy* meshProxy = static_cast<const GLMeshDeviceProxy*>(subMesh->GetMeshProxy());

			glBindVertexArray(RDI->PrimitivesQuad->VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);

			++i;
		}
	}
}
