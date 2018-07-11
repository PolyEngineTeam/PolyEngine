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

	struct ENGINE_DLLEXPORT Material
	{
		Material()
			: Emissive(0.0f, 0.0f, 0.0f), Albedo(1.0f, 1.0f, 1.0f), Roughness(1.0f), Metallic(1.0f), OpacityMaskThreshold(0.5f)
		{}

		Material(const Color& emissive, const Color& albedo, float roughness, float metallic, float opacityMaskThreshold)
			: Emissive(emissive), Albedo(albedo), Roughness(roughness), Metallic(metallic), OpacityMaskThreshold(opacityMaskThreshold)
		{}

		Color Emissive;
		Color Albedo;
		float Roughness;
		float Metallic;
		float OpacityMaskThreshold;
	};

	class ENGINE_DLLEXPORT MeshRenderingComponent : public ComponentBase
	{
	public:
		MeshRenderingComponent(const String& meshPath, eResourceSource source);
		virtual ~MeshRenderingComponent();

		const MeshResource* GetMesh() const { return Mesh; }
		const Material& GetMaterial(int i) const { return Materials[i]; }
		void SetMaterial(size_t i, const Material& value) { Materials[i] = value; }
		bool GetIsWireframe() const { return IsWireframe; }
		void SetIsWireframe(bool value) { IsWireframe = value; }
		eShadingModel GetShadingModel() const { return ShadingModel; }
		void SetShadingModel(eShadingModel value) { ShadingModel = value; }
		bool IsTransparent() const { return Materials.GetCapacity() > 0 && Materials[0].Albedo.A < 1.0f; } // HACK replace with better solution for translucent objects.

	private:
		MeshResource* Mesh = nullptr;
		Dynarray<Material> Materials;
		eShadingModel ShadingModel = eShadingModel::PBR;
		bool IsWireframe = false;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, MeshRenderingComponent)
}