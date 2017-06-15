#pragma once

#include "ComponentBase.hpp"
#include "RenderingSystem.hpp"
#include "MeshResource.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT MeshRenderingComponent : public ComponentBase
	{
		friend void RenderingSystem::RenderingPhase(World*);
	public:
		MeshRenderingComponent(const String& meshPath);
		virtual ~MeshRenderingComponent();

		const MeshResource* GetMesh() const { return Mesh; }
	private:
		MeshResource* Mesh = nullptr;
	};
}