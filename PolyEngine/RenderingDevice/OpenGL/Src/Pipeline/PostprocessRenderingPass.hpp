#pragma once

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
	class PostprocessRenderingPass : public RenderingPassBase
	{
	public:
		PostprocessRenderingPass(const GLRenderingDevice* rdi, const String& fragment);
		PostprocessRenderingPass(const GLRenderingDevice* rdi, const String& geometry, const String& fragment);
	
	protected:
		virtual void OnRun(Scene* world, const CameraComponent* camera, const AARect& rect, ePassType passType) override;
	};
}