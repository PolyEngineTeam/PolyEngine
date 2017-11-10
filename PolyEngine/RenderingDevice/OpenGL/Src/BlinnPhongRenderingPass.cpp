#include "BlinnPhongRenderingPass.hpp"

#include "GLMeshDeviceProxy.hpp"
#include "GLTextureDeviceProxy.hpp"

#include <World.hpp>
#include <CameraComponent.hpp>
#include <TransformComponent.hpp>
#include <MeshRenderingComponent.hpp>
#include <LightSourceComponent.hpp>
#include <MovementSystem.hpp>
#include <TimeWorldComponent.hpp>


using namespace Poly;

const size_t MAX_LIGHT_COUNT_POINT = 8;
const size_t MAX_LIGHT_COUNT_DIRECTIONAL = 8;
const size_t MAX_LIGHT_COUNT_SPOT = 8;

BlinnPhongRenderingPass::BlinnPhongRenderingPass()
: RenderingPassBase("Shaders/blinn-phongVert.shader", "Shaders/blinn-phongFrag.shader")
{
	CreateDummyTexture();

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

void BlinnPhongRenderingPass::OnRun(World* world, const CameraComponent* camera, const AARect& /*rect*/)
{

	GetProgram().BindProgram();
	const Matrix& mvp = camera->GetMVP();
	
	float time = (float)(world->GetWorldComponent<TimeWorldComponent>()->GetGameplayTime());
	GetProgram().SetUniform("uTime", time);

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
		GetProgram().SetUniform(baseName + "Range", pointLightCmp->GetRange());
		GetProgram().SetUniform(baseName + "Position", transformCmp->GetGlobalTranslation());
		GetProgram().SetUniform(baseName + "Base.Color", pointLightCmp->GetColor());
		GetProgram().SetUniform(baseName + "Base.Intensity", pointLightCmp->GetIntensity());

		++pointLightsCount;
		if (pointLightsCount == MAX_LIGHT_COUNT_POINT)
			break;
	}
	GetProgram().SetUniform("uPointLightCount", pointLightsCount);

	int spotLightsCount = 0;
	for (auto componentsTuple : world->IterateComponents<SpotLightComponent, TransformComponent>())
	{
		SpotLightComponent* spotLightCmp = std::get<SpotLightComponent*>(componentsTuple);
		TransformComponent* transformCmp = std::get<TransformComponent*>(componentsTuple);

		String baseName = String("uSpotLight[") + String::From(spotLightsCount) + String("].");
		GetProgram().SetUniform(baseName + "Range", spotLightCmp->GetRange());
		GetProgram().SetUniform(baseName + "CutOff", Cos(1.0_deg * spotLightCmp->GetCutOff()));
		GetProgram().SetUniform(baseName + "OuterCutOff", Cos(1.0_deg * spotLightCmp->GetOuterCutOff()));
		GetProgram().SetUniform(baseName + "Position", transformCmp->GetGlobalTranslation());
		GetProgram().SetUniform(baseName + "Direction", MovementSystem::GetGlobalForward(transformCmp));
		GetProgram().SetUniform(baseName + "Base.Color", spotLightCmp->GetColor());
		GetProgram().SetUniform(baseName + "Base.Intensity", spotLightCmp->GetIntensity());

		gConsole.LogInfo("BlinnPhongRenderingPass::OnRun: spotlight[{}]: pos: {}, dir: {}",
			spotLightsCount, transformCmp->GetGlobalTranslation(), MovementSystem::GetGlobalForward(transformCmp));

		++spotLightsCount;
		if (spotLightsCount == MAX_LIGHT_COUNT_SPOT)
			break;
	}
	GetProgram().SetUniform("uSpotLightCount", spotLightsCount);

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

			const Poly::TextureResource* DiffuseTexture = subMesh->GetMeshData().GetDiffTexture();
			GLuint TextureID = DiffuseTexture == nullptr
				? WhiteDummyTexture
				: static_cast<const GLTextureDeviceProxy*>(DiffuseTexture->GetTextureProxy())->GetTextureID();

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, TextureID);

			glDrawElements(GL_TRIANGLES, (GLsizei)subMesh->GetMeshData().GetTriangleCount() * 3, GL_UNSIGNED_INT, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindVertexArray(0);

			++i;
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void Poly::BlinnPhongRenderingPass::CreateDummyTexture()
{

	glGenTextures(1, &WhiteDummyTexture);

	GLubyte data[] = { 255, 255, 255, 255 };

	glBindTexture(GL_TEXTURE_2D, WhiteDummyTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
}
