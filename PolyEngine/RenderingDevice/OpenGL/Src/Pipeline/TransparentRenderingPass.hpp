#pragma once

#include <pe/Defines.hpp>
#include <Pipeline/RenderingPassBase.hpp>
#include <Proxy/GLShaderProgram.hpp>
#include <Common/PrimitiveQuad.hpp>

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
	class TransparentRenderingPass : public RenderingPassBase
	{
	public:
		TransparentRenderingPass(const GLRenderingDevice* rdi);

	protected:

		void OnRun(Scene* world, const CameraComponent* camera, const ::pe::core::math::AARect& rect, ePassType passType) override;
	};
}