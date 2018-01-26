#include "EnginePCH.hpp"

#include "ParticlesComponent.hpp"
#include "CameraComponent.hpp"
#include "ResourceManager.hpp"

using namespace Poly;

ParticlesComponent::ParticlesComponent()
{
	// Mesh = ResourceManager<MeshResource>::Load(meshPath, source);
	// Materials.Resize(Mesh->GetSubMeshes().GetSize());
}

Poly::ParticlesComponent::~ParticlesComponent()
{
	// if (Mesh)
	// 	ResourceManager<MeshResource>::Release(Mesh);
}
