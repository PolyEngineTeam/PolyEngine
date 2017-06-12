#include "EnginePCH.hpp"

#include "CameraComponent.hpp"
#include "MeshRenderingComponent.hpp"
#include "ResourceManager.hpp"

using namespace Poly;

MeshRenderingComponent::MeshRenderingComponent(const String& meshPath)
{
	Mesh = ResourceManager<MeshResource>::Load(meshPath);
}

Poly::MeshRenderingComponent::~MeshRenderingComponent()
{
	if (Mesh)
		ResourceManager<MeshResource>::Release(Mesh);
}
