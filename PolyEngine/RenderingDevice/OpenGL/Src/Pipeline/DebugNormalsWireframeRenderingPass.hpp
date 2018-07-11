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
	class DebugNormalsWireframeRenderingPass : public RenderingPassBase
	{
	public:
		DebugNormalsWireframeRenderingPass(const GLRenderingDevice* rdi);

	protected:
		virtual void OnRun(World* world, const CameraComponent* camera, const AARect& rect, ePassType passType = ePassType::BY_MATERIAL) override;
	};
}