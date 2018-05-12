#pragma once

#include "Pipeline/RenderingPassBase.hpp"
#include "Common/GLShaderProgram.hpp"

namespace Poly
{
	class Scene;
	class RenderingTargetBase;
	class GLTextureDeviceProxy;
	class AARect;

	//------------------------------------------------------------------------------
	class DebugNormalsWireframeRenderingPass : public RenderingPassBase
	{
	public:
		DebugNormalsWireframeRenderingPass();

	protected:
		virtual void OnRun(Scene* world, const CameraComponent* camera, const AARect& rect, ePassType passType = ePassType::BY_MATERIAL) override;
	};
}