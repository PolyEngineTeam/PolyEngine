#pragma once

#include "Pipeline/RenderingPassBase.hpp"
#include "Proxy/GLShaderProgram.hpp"

namespace Poly
{
	class World;
	class RenderingTargetBase;
	class GLTextureDeviceProxy;
	class AARect;

	//------------------------------------------------------------------------------
	class UnlitRenderingPass : public RenderingPassBase
	{
	public:
		UnlitRenderingPass(const GLRenderingDevice* rdi);

	protected:
		void OnRun(World* world, const CameraComponent* camera, const AARect& rect, ePassType passType) override;
	};
}