#pragma once

#include "ComponentBase.hpp"
#include "RenderingSystem.hpp"
#include "GLMeshResource.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT MeshRenderingComponent : public ComponentBase
	{
		friend void RenderingSystem::RenderingPhase(World*);
	public:
		MeshRenderingComponent(const String& meshPath);
		virtual ~MeshRenderingComponent();

		const GLMeshResource* GetMesh() const { return Mesh; }
	private:
		GLMeshResource* Mesh = nullptr;
	};
}