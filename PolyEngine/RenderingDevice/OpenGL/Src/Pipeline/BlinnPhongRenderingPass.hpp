#pragma once

#include "Pipeline/RenderingPassBase.hpp"
#include "Common/GLShaderProgram.hpp"

namespace Poly
{
	class World;
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

		void OnRun(World* world, const CameraComponent* camera, const AARect& rect, ePassType passType) override;
	};
}