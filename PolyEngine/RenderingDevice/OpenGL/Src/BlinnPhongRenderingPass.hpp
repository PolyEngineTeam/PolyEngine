#pragma once

#include "RenderingPassBase.hpp"
#include "GLShaderProgram.hpp"

namespace Poly
{
	class World;
	class RenderingTargetBase;
	class GLTextureDeviceProxy;
	class AARect;

	//------------------------------------------------------------------------------
	class BlinnPhongRenderingPass : public RenderingPassBase
	{
	public:
		BlinnPhongRenderingPass();

	protected:

		void OnRun(World* world, const CameraComponent* camera, const AARect& rect, ePassType passType) override;
	};
}