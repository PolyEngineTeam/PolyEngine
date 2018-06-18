#pragma once

#include "ECS/ComponentBase.hpp"
#include "RenderingSystem.hpp"
#include "Resources/MeshResource.hpp"

namespace Poly {

	enum class eShadingModel
	{
		NONE,
		PHONG,
		UNLIT,
		PBR,
		_COUNT
	};

	struct ENGINE_DLLEXPORT PBRMaterial
	{
		PBRMaterial()
			: Emissive(0.0f, 0.0f, 0.0f), Albedo(1.0f, 1.0f, 1.0f), Roughness(1.0f), Metallic(1.0f) {}

		PBRMaterial(const Color& emissive, const Color& albedo, float roughness, float metallic)
			: Emissive(emissive), Albedo(albedo), Roughness(roughness), Metallic(metallic) {}

		Color Emissive;
		Color Albedo;
		float Roughness;
		float Metallic;
	};

	class ENGINE_DLLEXPORT MeshRenderingComponent : public ComponentBase
	{
		friend void RenderingSystem::RenderingPhase(World*);
	public:
		MeshRenderingComponent(const String& meshPath, eResourceSource source);
		virtual ~MeshRenderingComponent();

		const MeshResource* GetMesh() const { return Mesh; }
		const PBRMaterial& GetPBRMaterial(int i) const { return PBRMaterials[i]; }
		void SetPBRMaterial(int i, const PBRMaterial& value) { PBRMaterials[i] = value; }
		bool GetIsWireframe() const { return IsWireframe; }
		void SetIsWireframe(bool value) { IsWireframe = value; }
		eShadingModel GetShadingModel() const { return ShadingModel; }
		void SetShadingModel(eShadingModel value) { ShadingModel = value; }
		bool IsTransparent() const { return PBRMaterials.GetCapacity() > 0 && PBRMaterials[0].Albedo.A < 1.0f; } // HACK replace with better solution for translucent objects.

	private:
		MeshResource* Mesh = nullptr;
		Dynarray<PBRMaterial> PBRMaterials;
		eShadingModel ShadingModel = eShadingModel::PBR;
		bool IsWireframe = false;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, MeshRenderingComponent)
}