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
	class RenderingTargetBase;
	class GLTextureDeviceProxy;

	//------------------------------------------------------------------------------
	class Text2DRenderingPass : public RenderingPassBase
	{
	public:
		Text2DRenderingPass(const GLRenderingDevice* rdi);

	protected:
		void OnRun(Scene* world, const CameraComponent* camera, const core::math::AARect& rect, ePassType passType) override;
	};
}