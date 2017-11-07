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
const size_t MAX_LIGHT_COUNT_POINT = 8;
const size_t MAX_LIGHT_COUNT_DIRECTIONAL = 8;

BlinnPhongRenderingPass::BlinnPhongRenderingPass()
: RenderingPassBase("Shaders/blinn-phongVert.shader", "Shaders/blinn-phongFrag.shader")
{
	GetProgram().RegisterUniform("vec4", "uCameraPosition");
	GetProgram().RegisterUniform("vec4", "uCameraForward");

	// GetProgram().SetUniform("uMaterial.Ambient", material.AmbientColor);
	// GetProgram().SetUniform("uMaterial.Diffuse", material.DiffuseColor);
	// GetProgram().SetUniform("uMaterial.Specular", material.SpecularColor);
	// GetProgram().SetUniform("uMaterial.Shininess", material.Shininess);

	GetProgram().RegisterUniform("vec4", "uMaterial.Ambient");
	GetProgram().RegisterUniform("vec4", "uMaterial.Diffuse");
	GetProgram().RegisterUniform("vec4", "uMaterial.Specular");
	GetProgram().RegisterUniform("float", "uMaterial.Shininess");

	GetProgram().RegisterUniform("vec4", "uAmbientLight.Color");
	GetProgram().RegisterUniform("float", "uAmbientLight.Intensity");

	for (size_t i = 0; i < MAX_LIGHT_COUNT_DIRECTIONAL; ++i)
	{
		String baseName = String("uDirectionalLight[") + String::From(static_cast<int>(i)) + String("].");
		GetProgram().RegisterUniform("vec4", baseName + "Base.Color");
		GetProgram().RegisterUniform("float", baseName + "Base.Intensity");
		GetProgram().RegisterUniform("vec4", baseName + "Direction");
	}

	GetProgram().RegisterUniform("int", "uDirectionalLightCount");

	for (size_t i = 0; i < MAX_LIGHT_COUNT_POINT; ++i)
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
	
	const TransformComponent* cameraTransCmp = camera->GetSibling<TransformComponent>();
	Vector CameraPos = cameraTransCmp->GetGlobalTranslation();
	Vector CameraDir = MovementSystem::GetGlobalForward(cameraTransCmp);
	GetProgram().SetUniform("uCameraPosition", CameraPos);
	GetProgram().SetUniform("uCameraForward", CameraDir);

	AmbientLightWorldComponent* ambientCmp = world->GetWorldComponent<AmbientLightWorldComponent>();
	GetProgram().SetUniform("uAmbientLight.Color", ambientCmp->GetColor());
	GetProgram().SetUniform("uAmbientLight.Intensity", ambientCmp->GetIntensity());

	int dirLightsCount = 0;
	for (auto componentsTuple : world->IterateComponents<DirectionalLightComponent, TransformComponent>())
	{
		DirectionalLightComponent* dirLightCmp = std::get<DirectionalLightComponent*>(componentsTuple);
		TransformComponent* transformCmp = std::get<TransformComponent*>(componentsTuple);
		String baseName = String("uDirectionalLight[") + String::From(dirLightsCount) + String("].");
		GetProgram().SetUniform(baseName + "Direction", MovementSystem::GetGlobalForward(transformCmp));
		GetProgram().SetUniform(baseName + "Base.Color", dirLightCmp->GetColor());
		GetProgram().SetUniform(baseName + "Base.Intensity", dirLightCmp->GetIntensity());
		
		++dirLightsCount;
		if (dirLightsCount == MAX_LIGHT_COUNT_DIRECTIONAL)
			break;
	}
	GetProgram().SetUniform("uDirectionalLightCount", dirLightsCount);
	
	int pointLightsCount = 0;
	for (auto componentsTuple : world->IterateComponents<PointLightComponent, TransformComponent>())
	{
		PointLightComponent* pointLightCmp = std::get<PointLightComponent*>(componentsTuple);
		TransformComponent* transformCmp = std::get<TransformComponent*>(componentsTuple);
	
		String baseName = String("uPointLight[") + String::From(pointLightsCount) + String("].");
	
		GetProgram().SetUniform(baseName + "Base.Color", pointLightCmp->GetColor());
		GetProgram().SetUniform(baseName + "Base.Intensity", pointLightCmp->GetIntensity());
		// GetProgram().SetUniform(baseName + "Attenuation", pointLightCmp->GetAttenuation());
		// float range = sqrt(((1.f / IntensityThreshold) - 1.0f)/ pointLightCmp->GetAttenuation());
		GetProgram().SetUniform(baseName + "Range", pointLightCmp->GetRange());
		GetProgram().SetUniform(baseName + "Position", transformCmp->GetGlobalTranslation());

		++pointLightsCount;
		if (pointLightsCount == MAX_LIGHT_COUNT_POINT)
			break;
	}
	GetProgram().SetUniform("uPointLightCount", pointLightsCount);

	// Render meshes
	for (auto componentsTuple : world->IterateComponents<MeshRenderingComponent, TransformComponent>())
	{
		const MeshRenderingComponent* meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);
		TransformComponent* transCmp = std::get<TransformComponent*>(componentsTuple);

		if (meshCmp->IsTransparent() || meshCmp->GetShadingModel() != eShadingModel::LIT)
		 	continue;

		Vector objPos = transCmp->GetGlobalTranslation();

		const Matrix& objTransform = transCmp->GetGlobalTransformationMatrix();
		Matrix screenTransform = mvp * objTransform;
		GetProgram().SetUniform("uTransform", objTransform);
		GetProgram().SetUniform("uMVPTransform", screenTransform);
		
		glPolygonMode(GL_FRONT_AND_BACK, meshCmp->GetIsWireframe() ? GL_LINE : GL_FILL);

		int i = 0;
		for (const MeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
		{
			PhongMaterial material = meshCmp->GetMaterial(i);
			GetProgram().SetUniform("uMaterial.Ambient", material.AmbientColor);
			GetProgram().SetUniform("uMaterial.Diffuse", material.DiffuseColor);
			GetProgram().SetUniform("uMaterial.Specular", material.SpecularColor);
			GetProgram().SetUniform("uMaterial.Shininess", material.Shininess);

			const GLMeshDeviceProxy* meshProxy = static_cast<const GLMeshDeviceProxy*>(subMesh->GetMeshProxy());

			glBindVertexArray(meshProxy->GetVAO());

			if (subMesh->GetMeshData().GetDiffTexture())
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, static_cast<const GLTextureDeviceProxy*>(subMesh->GetMeshData().GetDiffTexture()->GetTextureProxy())->GetTextureID());
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, 0);
			}

			glDrawElements(GL_TRIANGLES, (GLsizei)subMesh->GetMeshData().GetTriangleCount() * 3, GL_UNSIGNED_INT, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindVertexArray(0);

			++i;
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}
