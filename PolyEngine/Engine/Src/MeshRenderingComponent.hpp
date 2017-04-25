#pragma once

#include "ComponentBase.hpp"
#include "RenderingSystem.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT MeshRenderingComponent : public ComponentBase
	{
		friend void RenderingSystem::RenderingPhase(World*);
	public:
		MeshRenderingComponent();
		
	private:
		//TODO add mesh
	};
}