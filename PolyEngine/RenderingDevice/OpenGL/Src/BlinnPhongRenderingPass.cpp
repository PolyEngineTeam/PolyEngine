#include "BlinnPhongRenderingPass.hpp"

#include "GLMeshDeviceProxy.hpp"
#include "GLTextureDeviceProxy.hpp"

#include <World.hpp>
#include <CameraComponent.hpp>
#include <TransformComponent.hpp>
#include <MeshRenderingComponent.hpp>

using namespace Poly;

BlinnPhongRenderingPass::BlinnPhongRenderingPass()
: RenderingPassBase("Shaders/blinn-phongVert.shader", "Shaders/blinn-phongFrag.shader")
{

}

void BlinnPhongRenderingPass::OnRun(World* world, const CameraComponent* camera, const AABox& rect)
{
	GetProgram().BindProgram();
	const Matrix& mvp = camera->GetMVP();
	// Render meshes
	for (auto componentsTuple : world->IterateComponents<MeshRenderingComponent, TransformComponent>())
	{
		const MeshRenderingComponent* meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);
		const TransformComponent* transCmp = std::get<TransformComponent*>(componentsTuple);

		const Matrix& objTransform = transCmp->GetGlobalTransformationMatrix();
		Matrix screenTransform = mvp * objTransform;
		GetProgram().SetUniform("uTransform", screenTransform);
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
