#include "EnginePCH.hpp"

#include "Rendering/Camera/CameraComponent.hpp"
#include "Rendering/MeshRenderingComponent.hpp"
#include "Resources/ResourceManager.hpp"

using namespace Poly;

MeshRenderingComponent::MeshRenderingComponent(const String& meshPath, eResourceSource source)
{
	Mesh = ResourceManager<MeshResource>::Load(meshPath, source);
	if (Mesh) {
		Materials.Resize(Mesh->GetSubMeshes().GetSize());
	}
}

Poly::MeshRenderingComponent::~MeshRenderingComponent()
{
	if (Mesh)
		ResourceManager<MeshResource>::Release(Mesh);
}
