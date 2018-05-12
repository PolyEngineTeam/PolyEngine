#pragma once

#include "Pipeline/RenderingPassBase.hpp"
#include "Common/GLShaderProgram.hpp"
#include "Common/PostprocessQuad.hpp"

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
		PostprocessRenderingPass(const PostprocessQuad* quad, const String& fragment);
		PostprocessRenderingPass(const PostprocessQuad* quad, const String& geometry, const String& fragment);
	
	protected:
		virtual void OnRun(Scene* world, const CameraComponent* camera, const AARect& rect, ePassType passType) override;

		const PostprocessQuad* Quad;
	};
}