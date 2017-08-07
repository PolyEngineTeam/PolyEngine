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
	class DebugNormalsRenderingPass : public RenderingPassBase
	{
	public:
		DebugNormalsRenderingPass();

	protected:
		virtual void OnRun(World* world, const CameraComponent* camera, const AABox& rect) override;
	};
}