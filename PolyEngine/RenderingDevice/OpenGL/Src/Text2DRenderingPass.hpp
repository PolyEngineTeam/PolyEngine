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
	class Text2DRenderingPass : public RenderingPassBase
	{
	public:
		Text2DRenderingPass();

	protected:
		void OnRun(World* world, const CameraComponent* camera, const AABox& rect) override;
	};
}