#include "UnlitRenderingPass.hpp"

#include "GLMeshDeviceProxy.hpp"
#include "GLTextureDeviceProxy.hpp"

#include <World.hpp>
#include <CameraComponent.hpp>
#include <TransformComponent.hpp>
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

void UnlitRenderingPass::OnRun(World* world, const CameraComponent* camera, const AARect& /*rect*/)
{
	GetProgram().BindProgram();
	const Matrix& mvp = camera->GetMVP();
	
	// Render meshes
	for (auto componentsTuple : world->IterateComponents<MeshRenderingComponent, TransformComponent>())
	{
		const MeshRenderingComponent* meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);
		TransformComponent* transCmp = std::get<TransformComponent*>(componentsTuple);

		if (meshCmp->IsTransparent() || meshCmp->GetShadingModel() != eShadingModel::UNLIT)
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
			GetProgram().SetUniform("uColor", material.DiffuseColor);

			const GLMeshDeviceProxy* meshProxy = static_cast<const GLMeshDeviceProxy*>(subMesh->GetMeshProxy());
			glBindVertexArray(meshProxy->GetVAO());

			const Poly::TextureResource* DiffuseTexture = subMesh->GetMeshData().GetDiffTexture();
			GLuint TextureID = DiffuseTexture == nullptr
				? FallbackWhiteTexture
				: static_cast<const GLTextureDeviceProxy*>(DiffuseTexture->GetTextureProxy())->GetTextureID();

			glDrawElements(GL_TRIANGLES, (GLsizei)subMesh->GetMeshData().GetTriangleCount() * 3, GL_UNSIGNED_INT, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindVertexArray(0);

			++i;
		}
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}