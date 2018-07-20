#include "EnginePCH.hpp"

#include "Rendering/Camera/CameraComponent.hpp"
#include "Rendering/MeshRenderingComponent.hpp"
#include "Resources/ResourceManager.hpp"

using namespace Poly;

RTTI_DEFINE_TYPE(Poly::MeshRenderingComponent)

//------------------------------------------------------------------------------
Poly::MeshRenderingComponent::MeshRenderingComponent()
{
	Mesh = ResourceManager<MeshResource>::Load("Models/Player.obj", eResourceSource::ENGINE);
	if (Mesh) {
		size_t materialsNum = GetMesh()->GetSubMeshes().GetSize();
		Materials.Resize(materialsNum);
		for (size_t i = 0; i < materialsNum; ++i)
		{
			SetMaterial(i, Material());
		}
	}
}

//------------------------------------------------------------------------------
MeshRenderingComponent::MeshRenderingComponent(const String& meshPath, eResourceSource source)
{
	Mesh = ResourceManager<MeshResource>::Load(meshPath, source);
	if (Mesh) {
		size_t materialsNum = GetMesh()->GetSubMeshes().GetSize();
		Materials.Resize(materialsNum);
		for (size_t i = 0; i < materialsNum; ++i)
		{
			SetMaterial(i, Material());
		}
	}
}

//------------------------------------------------------------------------------
Poly::MeshRenderingComponent::~MeshRenderingComponent()
{
	if (Mesh)
		ResourceManager<MeshResource>::Release(Mesh);
}
