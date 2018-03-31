#pragma once

#include "Pipeline/RenderingPassBase.hpp"
#include "Common/GLShaderProgram.hpp"

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

		void CreateDummyTexture();

	private:
		GLuint WhiteDummyTexture;
	};
}