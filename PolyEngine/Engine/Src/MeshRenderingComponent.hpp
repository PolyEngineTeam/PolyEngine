#pragma once

#include "ComponentBase.hpp"
#include "RenderingSystem.hpp"
#include "MeshResource.hpp"

namespace Poly {

	struct ENGINE_DLLEXPORT PhongMaterial
	{
		PhongMaterial()
			: AmbientColor(1.0f, 1.0f, 1.0f), DiffuseColor(1.0f, 1.0f, 1.0f), SpecularColor(1.0f, 1.0f, 1.0f), Shininess(0.1f) {}

		PhongMaterial(const Color& ambient, const Color& diffuse, const Color& specular, float shininess)
			: AmbientColor(ambient), DiffuseColor(diffuse), SpecularColor(specular), Shininess(shininess) {}

		Color AmbientColor;
		Color DiffuseColor;
		Color SpecularColor;
		float Shininess;
	};

	class ENGINE_DLLEXPORT MeshRenderingComponent : public ComponentBase
	{
		friend void RenderingSystem::RenderingPhase(World*);
	public:
		MeshRenderingComponent(const String& meshPath, eResourceSource source, const PhongMaterial& material = {});
		virtual ~MeshRenderingComponent();

		const MeshResource* GetMesh() const { return Mesh; }
		const PhongMaterial& GetMaterial() const { return Material; }
		void SetMaterial(const PhongMaterial& value) { Material = value; }
		bool IsTransparent() const { return Material.DiffuseColor.A < 1.0f; } //HACK replace with better solution for transloucent objects.
	private:
		MeshResource* Mesh = nullptr;
		PhongMaterial Material;
	};
}