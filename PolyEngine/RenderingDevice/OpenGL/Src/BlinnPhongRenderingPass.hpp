#pragma once

#include "RenderingPassBase.hpp"
#include "GLShaderProgram.hpp"

namespace Poly
{
	class World;
	class RenderingTargetBase;
	class GLTextureDeviceProxy;
	class AABox;

	//------------------------------------------------------------------------------
	class BlinnPhongRenderingPass : public RenderingPassBase
	{
	public:
		BlinnPhongRenderingPass();

		void Run(World* world, const CameraComponent* camera, const AABox& rect) override;

	private:
		GLShaderProgram Program;
	};
}