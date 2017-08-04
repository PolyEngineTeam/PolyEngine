#pragma once

#include "ComponentBase.hpp"
#include "RenderingSystem.hpp"
#include "MeshResource.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT MeshRenderingComponent : public ComponentBase
	{
		friend void RenderingSystem::RenderingPhase(World*);
	public:
		MeshRenderingComponent(const String& meshPath, eResourceSource source, const Color& baseColor = Color(0.0f, 0.0f, 0.0f, 1.0f));
		virtual ~MeshRenderingComponent();

		const MeshResource* GetMesh() const { return Mesh; }
		const Color& GetBaseColor() const { return BaseColor; }
		bool IsTransparent() const { return BaseColor.A < 1.0f; }
	private:
		MeshResource* Mesh = nullptr;
		Color BaseColor;
	};
}