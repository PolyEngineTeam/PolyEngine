#pragma once

#include <pe/Defines.hpp>
#include <Pipeline/RenderingPassBase.hpp>
#include <Proxy/GLShaderProgram.hpp>

namespace pe::core::math
{
	class AARect;
}

namespace Poly
{
	class Scene;

	//------------------------------------------------------------------------------
	class DebugRenderingPass : public RenderingPassBase
	{
	public:
		DebugRenderingPass(const GLRenderingDevice* rdi);

	protected:
		void OnRun(Scene* world, const CameraComponent* camera, const ::pe::core::math::AARect& rect, ePassType passType) override final;
	};
}