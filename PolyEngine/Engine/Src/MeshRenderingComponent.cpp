#include "EnginePCH.hpp"

#include "CameraComponent.hpp"
#include "MeshRenderingComponent.hpp"
#include "ResourceManager.hpp"

using namespace Poly;

MeshRenderingComponent::MeshRenderingComponent(const String& meshPath, eResourceSource source, const Color& baseColor)
	: BaseColor(baseColor)
{
	Mesh = ResourceManager<MeshResource>::Load(meshPath, source);
}

Poly::MeshRenderingComponent::~MeshRenderingComponent()
{
	if (Mesh)
		ResourceManager<MeshResource>::Release(Mesh);
}
