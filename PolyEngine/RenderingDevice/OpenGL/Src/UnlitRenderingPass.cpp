#include "UnlitRenderingPass.hpp"

#include "GLMeshDeviceProxy.hpp"
#include "GLTextureDeviceProxy.hpp"

#include <Entity.hpp>
#include <World.hpp>
#include <CameraComponent.hpp>
#include <MeshRenderingComponent.hpp>
#include <MovementSystem.hpp>

using namespace Poly;

UnlitRenderingPass::UnlitRenderingPass()
	: RenderingPassBase("Shaders/unlitVert.shader", "Shaders/unlitFrag.shader")
{
	GetProgram().RegisterUniform("mat4", "uTransform");
	GetProgram().RegisterUniform("mat4", "uMVPTransform");
	GetProgram().RegisterUniform("vec4", "Color");
}

void UnlitRenderingPass::OnRun(World* world, const CameraComponent* camera, const AARect& /*rect*/, ePassType passType = ePassType::GLOBAL)
{
	GetProgram().BindProgram();
	const Matrix& ScreenFromWorld = camera->GetScreenFromWorld();
	
	// Render meshes
	for (auto componentsTuple : world->IterateComponents<MeshRenderingComponent>())
	{
		const MeshRenderingComponent* meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);

		if (passType == ePassType::BY_MATERIAL &&
			(meshCmp->IsTransparent() || meshCmp->GetShadingModel() != eShadingModel::UNLIT))
		{
			continue;
		}

		const EntityTransform& transform = meshCmp->GetTransform();
		const Matrix& WorldFromModel = transform.GetWorldFromModel();
		Matrix ScreenFromModel = ScreenFromWorld * WorldFromModel;
		GetProgram().SetUniform("uTransform", WorldFromModel);
		GetProgram().SetUniform("uMVPTransform", ScreenFromModel);
		
		if (passType == ePassType::BY_MATERIAL)
		{
			glPolygonMode(GL_FRONT_AND_BACK, meshCmp->GetIsWireframe() ? GL_LINE : GL_FILL);
		}

		int i = 0;
		for (const MeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
		{
			PhongMaterial material = meshCmp->GetMaterial(i);
			GetProgram().SetUniform("uColor", material.DiffuseColor);

			const GLMeshDeviceProxy* meshProxy = static_cast<const GLMeshDeviceProxy*>(subMesh->GetMeshProxy());
			glBindVertexArray(meshProxy->GetVAO());

			const Poly::TextureResource* DiffuseTexture = subMesh->GetMeshData().GetDiffTexture();
			GLuint TextureID = DiffuseTexture == nullptr
				? FallbackWhiteTexture
				: static_cast<const GLTextureDeviceProxy*>(DiffuseTexture->GetTextureProxy())->GetTextureID();

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, TextureID);

			glDrawElements(GL_TRIANGLES, (GLsizei)subMesh->GetMeshData().GetTriangleCount() * 3, GL_UNSIGNED_INT, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindVertexArray(0);

			++i;
		}
		
		if (passType == ePassType::BY_MATERIAL)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
}