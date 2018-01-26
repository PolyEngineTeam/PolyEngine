#pragma once

#include "ComponentBase.hpp"
#include "RenderingSystem.hpp"
#include "MeshResource.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT ParticlesComponent : public ComponentBase
	{
		// friend void ParticlesComponent::ParticlesComponent(World*);
	public:
		ParticlesComponent();
		virtual ~ParticlesComponent();

		// const MeshResource* GetMesh() const { return Mesh; }
		// const PhongMaterial& GetMaterial(int i) const { return Materials[i]; }
		// void SetMaterial(int i, const PhongMaterial& value) { Materials[i] = value; }
		// bool GetIsWireframe() const { return IsWireframe; }
		// void SetIsWireframe(bool value) { IsWireframe = value; }
		// eShadingModel GetShadingModel() const { return ShadingModel; }
		// void SetShadingModel(eShadingModel value) { ShadingModel = value; }
		// bool IsTransparent() const { return Materials[0].DiffuseColor.A < 1.0f; } // HACK replace with better solution for transloucent objects.

	private:
		// MeshResource* Mesh = nullptr;
		// Dynarray<PhongMaterial> Materials;
		// eShadingModel ShadingModel = eShadingModel::LIT;
		// bool IsWireframe = false;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, ParticlesComponent)
}