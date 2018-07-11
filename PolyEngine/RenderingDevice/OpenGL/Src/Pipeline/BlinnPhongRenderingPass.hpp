#pragma once

#include "Pipeline/RenderingPassBase.hpp"
#include "Proxy/GLShaderProgram.hpp"

namespace Poly
{
	class Scene;
	class AARect;
	class RenderingTargetBase;
	class GLTextureDeviceProxy;
	class GLRenderingDevice;

	//------------------------------------------------------------------------------
	class BlinnPhongRenderingPass : public RenderingPassBase
	{
	public:
		BlinnPhongRenderingPass(const GLRenderingDevice* rdi);

	protected:

		void OnRun(Scene* world, const CameraComponent* camera, const AARect& rect, ePassType passType) override;
	};
}