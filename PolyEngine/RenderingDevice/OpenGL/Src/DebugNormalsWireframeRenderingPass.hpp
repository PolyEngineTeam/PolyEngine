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
	class DebugNormalsWireframeRenderingPass : public RenderingPassBase
	{
	public:
		DebugNormalsWireframeRenderingPass();

	protected:
		virtual void OnRun(World* world, const CameraComponent* camera, const AARect& rect) override;
	};
}