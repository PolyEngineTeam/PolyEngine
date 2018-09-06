#pragma once

#include <Defines.hpp>
#include "Pipeline/RenderingPassBase.hpp"
#include "Proxy/GLShaderProgram.hpp"
#include "Common/PrimitiveQuad.hpp"

namespace Poly
{
	class Scene;
	class RenderingTargetBase;
	class GLTextureDeviceProxy;
	class AARect;

	//------------------------------------------------------------------------------
	class TransparentRenderingPass : public RenderingPassBase
	{
	public:
		TransparentRenderingPass(const GLRenderingDevice* rdi);

	protected:

		void OnRun(Scene* world, const CameraComponent* camera, const AARect& rect, ePassType passType) override;
	};
}