#include "BlinnPhongRenderingPass.hpp"

#include "GLMeshDeviceProxy.hpp"
#include "GLTextureDeviceProxy.hpp"

#include <World.hpp>
#include <CameraComponent.hpp>
#include <TransformComponent.hpp>
#include <MeshRenderingComponent.hpp>
#include <LightSourceComponent.hpp>
#include <MovementSystem.hpp>

using namespace Poly;

const float IntensityThreshold = 0.05f;
const size_t MAX_POINT_LIGHT_COUNT = 8;

BlinnPhongRenderingPass::BlinnPhongRenderingPass()
: RenderingPassBase("Shaders/blinn-phongVert.shader", "Shaders/blinn-phongFrag.shader")
{
	GetProgram().RegisterUniform("vec4", "uDiffuseLight.Color");
	GetProgram().RegisterUniform("float", "uDiffuseLight.Intensity");

	GetProgram().RegisterUniform("vec4", "uDirectionalLight.Base.Color");
	GetProgram().RegisterUniform("float", "uDirectionalLight.Base.Intensity");
	GetProgram().RegisterUniform("vec4", "uDirectionalLight.Direction");

	for (size_t i = 0; i < MAX_POINT_LIGHT_COUNT; ++i)
	{
		String baseName = String("uPointLight[") + String::From(static_cast<int>(i)) + String("].");
		GetProgram().RegisterUniform("vec4", baseName + "Base.Color");
		GetProgram().RegisterUniform("float", baseName + "Base.Intensity");
		GetProgram().RegisterUniform("vec4", baseName + "Position");
		GetProgram().RegisterUniform("float", baseName + "Attenuation");
		GetProgram().RegisterUniform("float", baseName + "Range");
	}

	GetProgram().RegisterUniform("int", "uPointLightCount");
}

void BlinnPhongRenderingPass::OnRun(World* world, const CameraComponent* camera, const AARect& /*rect*/)
{
	GetProgram().BindProgram();
	const Matrix& mvp = camera->GetMVP();

	DiffuseLightSourceWorldComponent* diffuseCmp = world->GetWorldComponent<DiffuseLightSourceWorldComponent>();

	GetProgram().SetUniform("uDiffuseLight.Color", diffuseCmp->GetColor());
	GetProgram().SetUniform("uDiffuseLight.Intensity", diffuseCmp->GetIntensity());

	GetProgram().SetUniform("uDirectionalLight.Base.Intensity", 0.f);
	for (auto componentsTuple : world->IterateComponents<DirectionalLightSourceComponent, TransformComponent>())
	{
		DirectionalLightSourceComponent* dirLightCmp = std::get<DirectionalLightSourceComponent*>(componentsTuple);
		TransformComponent* transformCmp = std::get<TransformComponent*>(componentsTuple);

		GetProgram().SetUniform("uDirectionalLight.Base.Color", dirLightCmp->GetColor());
		GetProgram().SetUniform("uDirectionalLight.Base.Intensity", dirLightCmp->GetIntensity());
		GetProgram().SetUniform("uDirectionalLight.Direction", MovementSystem::GetGlobalForward(transformCmp));
		// use only first on scene
		break;
	}

	int pointLightsCount = 0;
	for (auto componentsTuple : world->IterateComponents<PointLightSourceComponent, TransformComponent>())
	{
		PointLightSourceComponent* pointLightCmp = std::get<PointLightSourceComponent*>(componentsTuple);
		TransformComponent* transformCmp = std::get<TransformComponent*>(componentsTuple);

		String baseName = String("uPointLight[") + String::From(pointLightsCount) + String("].");

		GetProgram().SetUniform(baseName + "Base.Color", pointLightCmp->GetColor());
		GetProgram().SetUniform(baseName + "Base.Intensity", pointLightCmp->GetIntensity());
		GetProgram().SetUniform(baseName + "Position", transformCmp->GetGlobalTranslation());
		GetProgram().SetUniform(baseName + "Attenuation", pointLightCmp->GetAttenuation());
		float range = sqrt(((1.f / IntensityThreshold) - 1.0f)/ pointLightCmp->GetAttenuation());
		GetProgram().SetUniform(baseName + "Range", range);
		// use only first on scene
		++pointLightsCount;
		if (pointLightsCount == MAX_POINT_LIGHT_COUNT)
			break;
	}

	GetProgram().SetUniform("uPointLightCount", pointLightsCount);

	const float cameraHeight = 16.f + 1.f;
	float verticalSpan = cameraHeight / 2.0f;
	float horizontalSpan = (cameraHeight * camera->GetAspect()) / 2.0f;
	Vector cameraPos = camera->GetSibling<TransformComponent>()->GetGlobalTranslation();



	// Render meshes
	for (auto componentsTuple : world->IterateComponents<MeshRenderingComponent, TransformComponent>())
	{
		const MeshRenderingComponent* meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);
		TransformComponent* transCmp = std::get<TransformComponent*>(componentsTuple);

		if (meshCmp->IsTransparent())
			continue;

		Vector objPos = transCmp->GetGlobalTranslation();

		bool shouldCull = objPos.Y > cameraPos.Y + verticalSpan;
		shouldCull = shouldCull || objPos.Y < cameraPos.Y - verticalSpan;
		shouldCull = shouldCull || objPos.X > cameraPos.X + horizontalSpan;
		shouldCull = shouldCull || objPos.X < cameraPos.X - horizontalSpan;
		if (shouldCull)
			continue;


		const Matrix& objTransform = transCmp->GetGlobalTransformationMatrix();
		Matrix screenTransform = mvp * objTransform;
		GetProgram().SetUniform("uTransform", objTransform);
		GetProgram().SetUniform("uMVPTransform", screenTransform);
		GetProgram().SetUniform("uBaseColor", meshCmp->GetBaseColor());
		for (const MeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
		{
			const GLMeshDeviceProxy* meshProxy = static_cast<const GLMeshDeviceProxy*>(subMesh->GetMeshProxy());

			glBindVertexArray(meshProxy->GetVAO());

			if (subMesh->GetMeshData().GetDiffTexture())
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, static_cast<const GLTextureDeviceProxy*>(subMesh->GetMeshData().GetDiffTexture()->GetTextureProxy())->GetTextureID());
			}

			glDrawElements(GL_TRIANGLES, (GLsizei)subMesh->GetMeshData().GetTriangleCount() * 3, GL_UNSIGNED_INT, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindVertexArray(0);
		}
	}
}
