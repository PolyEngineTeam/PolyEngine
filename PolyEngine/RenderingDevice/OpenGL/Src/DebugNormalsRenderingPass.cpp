#include "DebugNormalsRenderingPass.hpp"

#include "GLMeshDeviceProxy.hpp"
#include "GLTextureDeviceProxy.hpp"

#include <World.hpp>
#include <CameraComponent.hpp>
#include <TransformComponent.hpp>
#include <MeshRenderingComponent.hpp>

using namespace Poly;

DebugNormalsRenderingPass::DebugNormalsRenderingPass()
: RenderingPassBase("Shaders/debugNormalsVert.shader", "Shaders/debugNormalsGeom.shader", "Shaders/debugNormalsFrag.shader")
{

}

void DebugNormalsRenderingPass::OnRun(World* world, const CameraComponent* camera, const AARect& rect)
{
	const Matrix& mModelView = camera->GetMVP();
	const Matrix& mProjection = camera->GetProjectionMatrix();

	GetProgram().BindProgram();
	GetProgram().SetUniform("u_projection", mProjection);

	for (auto componentsTuple : world->IterateComponents<MeshRenderingComponent, TransformComponent>())
	{
		const MeshRenderingComponent* meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);
		const TransformComponent* transCmp = std::get<TransformComponent*>(componentsTuple);

		const Matrix& objTransform = transCmp->GetGlobalTransformationMatrix();
		Matrix MVPTransform = mModelView * objTransform;
		Matrix mNormalMatrix = (mModelView * objTransform).GetInversed().GetTransposed();
		GetProgram().SetUniform("u_MVP", MVPTransform);
		GetProgram().SetUniform("u_normalMatrix4x4", mNormalMatrix);
		for (const MeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
		{
			const GLMeshDeviceProxy* meshProxy = static_cast<const GLMeshDeviceProxy*>(subMesh->GetMeshProxy());
			glBindVertexArray(meshProxy->GetVAO());
			glDrawElements(GL_TRIANGLES, (GLsizei)subMesh->GetMeshData().GetTriangleCount() * 3, GL_UNSIGNED_INT, NULL);
			glBindVertexArray(0);
		}
	}
}
