#include "PolyRenderingDeviceGLPCH.hpp"

#include "Pipeline/DebugNormalsWireframeRenderingPass.hpp"
#include "Proxy/GLMeshDeviceProxy.hpp"
#include "Proxy/GLTextureDeviceProxy.hpp"

using namespace Poly;

DebugNormalsWireframeRenderingPass::DebugNormalsWireframeRenderingPass(const GLRenderingDevice* rdi)
: RenderingPassBase(rdi, "Shaders/debugNormals.vert.glsl", "Shaders/debugNormals.geom.glsl", "Shaders/debugNormals.frag.glsl")
{

}

void DebugNormalsWireframeRenderingPass::OnRun(Scene* world, const CameraComponent* camera, const AARect& /*rect*/, ePassType /*passType = ePassType::BY_MATERIAL*/)
{
	const Matrix& mModelView = camera->GetClipFromWorld();
	const Matrix& mProjection = camera->GetClipFromView();

	GetProgram().BindProgram();
	GetProgram().SetUniform("u_projection", mProjection);

	for (auto componentsTuple : world->IterateComponents<MeshRenderingComponent>())
	{
		const MeshRenderingComponent* meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);
		const EntityTransform& trans = meshCmp->GetTransform();

		const Matrix& objTransform = trans.GetWorldFromModel();
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
