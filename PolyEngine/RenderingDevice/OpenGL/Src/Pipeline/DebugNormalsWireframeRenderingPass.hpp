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
	class DebugNormalsWireframeRenderingPass : public RenderingPassBase
	{
	public:
		DebugNormalsWireframeRenderingPass(const GLRenderingDevice* rdi);

	protected:
		virtual void OnRun(Scene* world, const CameraComponent* camera, const core::math::AARect& rect, ePassType passType = ePassType::BY_MATERIAL) override;
	};
}