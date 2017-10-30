#pragma once

#include "ComponentBase.hpp"
#include "RenderingSystem.hpp"
#include "MeshResource.hpp"
#include "CoreConfig.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT MeshRenderingComponent : public ComponentBase
	{
		friend void RenderingSystem::RenderingPhase(World*);
	public:
		MeshRenderingComponent(const String& meshPath, eResourceSource source, const Color& baseColor = Color(0,0,0,1));
		virtual ~MeshRenderingComponent();

		const MeshResource* GetMesh() const { return Mesh; }
		const Color& GetBaseColor() const { return BaseColor; }
		bool IsTransparent() const { return BaseColor.A < 1.0f; } //HACK replace with better solution for transloucent objects.
	private:
		MeshResource* Mesh = nullptr;
		Color BaseColor;
	};
}