#include "Pipeline/BlinnPhongRenderingPass.hpp"

#include "Proxy/GLMeshDeviceProxy.hpp"
#include "Proxy/GLTextureDeviceProxy.hpp"
#include <GLRenderingDevice.hpp>

#include <ECS/World.hpp>
#include <Rendering/Camera/CameraComponent.hpp>
#include <Rendering/MeshRenderingComponent.hpp>
#include <Rendering/Lighting/LightSourceComponent.hpp>
#include <Movement/MovementSystem.hpp>


using namespace Poly;

const size_t MAX_LIGHT_COUNT_POINT = 8;
const size_t MAX_LIGHT_COUNT_DIRECTIONAL = 8;
const size_t MAX_LIGHT_COUNT_SPOT = 8;

BlinnPhongRenderingPass::BlinnPhongRenderingPass(const GLRenderingDevice* rdi)
: RenderingPassBase(rdi, "Shaders/blinn-phong.vert.glsl", "Shaders/blinn-phong.frag.glsl")
{
	GetProgram().RegisterUniform("vec4", "uCameraPosition");
	GetProgram().RegisterUniform("vec4", "uCameraForward");

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

	for (size_t i = 0; i < MAX_LIGHT_COUNT_SPOT; ++i)
	{
		String baseName = String("uSpotLight[") + String::From(static_cast<int>(i)) + String("].");
		GetProgram().RegisterUniform("vec4", baseName + "Base.Color");
		GetProgram().RegisterUniform("float", baseName + "Base.Intensity");
		GetProgram().RegisterUniform("vec4", baseName + "Position");
		GetProgram().RegisterUniform("vec4", baseName + "Direction");
		GetProgram().RegisterUniform("float", baseName + "Range");
		GetProgram().RegisterUniform("float", baseName + "CutOff");
		GetProgram().RegisterUniform("float", baseName + "OuterCutOff");
	}

	GetProgram().RegisterUniform("int", "uSpotLightCount");
}

void BlinnPhongRenderingPass::OnRun(Scene* world, const CameraComponent* camera, const AARect& /*rect*/, ePassType passType = ePassType::GLOBAL)
{

	GetProgram().BindProgram();
	const Matrix& mvp = camera->GetClipFromWorld();
	
	const EntityTransform& cameraTrans = camera->GetTransform();
	Vector CameraPos = cameraTrans.GetGlobalTranslation();
	Vector CameraDir = MovementSystem::GetGlobalForward(cameraTrans);
	GetProgram().SetUniform("uCameraPosition", CameraPos);
	GetProgram().SetUniform("uCameraForward", CameraDir);

	AmbientLightWorldComponent* ambientCmp = world->GetWorldComponent<AmbientLightWorldComponent>();
	GetProgram().SetUniform("uAmbientLight.Color", ambientCmp->GetColor());
	GetProgram().SetUniform("uAmbientLight.Intensity", ambientCmp->GetIntensity());

	int dirLightsCount = 0;
	for (const auto& componentsTuple : world->IterateComponents<DirectionalLightComponent>())
	{
		DirectionalLightComponent* dirLightCmp = std::get<DirectionalLightComponent*>(componentsTuple);
		const EntityTransform& transform = dirLightCmp->GetTransform();
		String baseName = String("uDirectionalLight[") + String::From(dirLightsCount) + String("].");
		GetProgram().SetUniform(baseName + "Direction", MovementSystem::GetGlobalForward(transform));
		GetProgram().SetUniform(baseName + "Base.Color", dirLightCmp->GetColor());
		GetProgram().SetUniform(baseName + "Base.Intensity", dirLightCmp->GetIntensity());
		
		++dirLightsCount;
		if (dirLightsCount == MAX_LIGHT_COUNT_DIRECTIONAL)
			break;
	}
	GetProgram().SetUniform("uDirectionalLightCount", dirLightsCount);
	
	int pointLightsCount = 0;
	for (const auto& componentsTuple : world->IterateComponents<PointLightComponent>())
	{
		PointLightComponent* pointLightCmp = std::get<PointLightComponent*>(componentsTuple);
		const EntityTransform& transform = pointLightCmp->GetTransform();
	
		String baseName = String("uPointLight[") + String::From(pointLightsCount) + String("].");
		GetProgram().SetUniform(baseName + "Range", pointLightCmp->GetRange());
		GetProgram().SetUniform(baseName + "Position", transform.GetGlobalTranslation());
		GetProgram().SetUniform(baseName + "Base.Color", pointLightCmp->GetColor());
		GetProgram().SetUniform(baseName + "Base.Intensity", pointLightCmp->GetIntensity());

		++pointLightsCount;
		if (pointLightsCount == MAX_LIGHT_COUNT_POINT)
			break;
	}
	GetProgram().SetUniform("uPointLightCount", pointLightsCount);

	int spotLightsCount = 0;
	for (const auto& componentsTuple : world->IterateComponents<SpotLightComponent>())
	{
		SpotLightComponent* spotLightCmp = std::get<SpotLightComponent*>(componentsTuple);
		const EntityTransform& transform = spotLightCmp->GetTransform();

		String baseName = String("uSpotLight[") + String::From(spotLightsCount) + String("].");
		GetProgram().SetUniform(baseName + "Range", spotLightCmp->GetRange());
		GetProgram().SetUniform(baseName + "CutOff", Cos(1.0_deg * spotLightCmp->GetCutOff()));
		GetProgram().SetUniform(baseName + "OuterCutOff", Cos(1.0_deg * spotLightCmp->GetOuterCutOff()));
		GetProgram().SetUniform(baseName + "Position", transform.GetGlobalTranslation());
		GetProgram().SetUniform(baseName + "Direction", MovementSystem::GetGlobalForward(transform));
		GetProgram().SetUniform(baseName + "Base.Color", spotLightCmp->GetColor());
		GetProgram().SetUniform(baseName + "Base.Intensity", spotLightCmp->GetIntensity());

		++spotLightsCount;
		if (spotLightsCount == MAX_LIGHT_COUNT_SPOT)
			break;
	}
	GetProgram().SetUniform("uSpotLightCount", spotLightsCount);

	// Render meshes
	for (const auto& componentsTuple : world->IterateComponents<MeshRenderingComponent>())
	{
		const MeshRenderingComponent* meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);
		const EntityTransform& transform = meshCmp->GetTransform();

		if ( passType == ePassType::BY_MATERIAL &&
			(meshCmp->IsTransparent() || meshCmp->GetShadingModel() != eShadingModel::PHONG))
		{
			continue;
		}

		const Matrix& objTransform = transform.GetWorldFromModel();
		const Matrix& localTransform = transform.GetParentFromModel();
		Matrix screenTransform = mvp * objTransform;
		GetProgram().SetUniform("uTransform", objTransform);
		GetProgram().SetUniform("uLocalTransform", localTransform);
		GetProgram().SetUniform("uMVPTransform", screenTransform);
		
		glPolygonMode(GL_FRONT_AND_BACK, meshCmp->GetIsWireframe() ? GL_LINE : GL_FILL);

		int i = 0;
		for (const MeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
		{
			Material material = meshCmp->GetMaterial(i);
			GetProgram().SetUniform("uMaterial.Ambient", Color::BLACK);
			GetProgram().SetUniform("uMaterial.Diffuse", material.Albedo);
			GetProgram().SetUniform("uMaterial.Specular", material.Albedo);
			GetProgram().SetUniform("uMaterial.Shininess", 16.0f);

			const GLMeshDeviceProxy* meshProxy = static_cast<const GLMeshDeviceProxy*>(subMesh->GetMeshProxy());
			glBindVertexArray(meshProxy->GetVAO());

			const Poly::TextureResource* DiffuseTexture = subMesh->GetMeshData().GetAlbedoMap();
			GLuint TextureID = DiffuseTexture == nullptr
				? RDI->FallbackWhiteTexture
				: static_cast<const GLTextureDeviceProxy*>(DiffuseTexture->GetTextureProxy())->GetTextureID();

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, TextureID);
			GetProgram().SetUniform("uDiffuseTexture", 0);

			const Poly::TextureResource* SpecularMap = subMesh->GetMeshData().GetRoughnessMap();
			GLuint SpecularMapID = SpecularMap == nullptr
				? RDI->FallbackWhiteTexture
				: static_cast<const GLTextureDeviceProxy*>(SpecularMap->GetTextureProxy())->GetTextureID();

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, SpecularMapID);
			GetProgram().SetUniform("uSpecularMap", 1);

			const Poly::TextureResource* NormalMap = subMesh->GetMeshData().GetNormalMap();
			GLuint NormalMapID = NormalMap == nullptr
				? RDI->FallbackNormalMap
				: static_cast<const GLTextureDeviceProxy*>(NormalMap->GetTextureProxy())->GetTextureID();

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, NormalMapID);
			GetProgram().SetUniform("uNormalMap", 2);

			glDrawElements(GL_TRIANGLES, (GLsizei)subMesh->GetMeshData().GetTriangleCount() * 3, GL_UNSIGNED_INT, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindVertexArray(0);

			++i;
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}