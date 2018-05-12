#pragma once

#include "Pipeline/RenderingPassBase.hpp"
#include "Common/GLShaderProgram.hpp"

namespace Poly
{
	class Scene;
	class AARect;

	//------------------------------------------------------------------------------
	class DebugRenderingPass : public RenderingPassBase
	{
	public:
		DebugRenderingPass();

	protected:
		void OnRun(Scene* world, const CameraComponent* camera, const AARect& rect, ePassType passType) override final;
	};
}