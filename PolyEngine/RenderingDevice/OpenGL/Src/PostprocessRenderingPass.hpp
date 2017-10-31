#pragma once

#include "RenderingPassBase.hpp"
#include "GLShaderProgram.hpp"
#include "PostprocessQuad.hpp"

namespace Poly
{
	class World;
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
		virtual void OnRun(World* world, const CameraComponent* camera, const AARect& rect) override;

		const PostprocessQuad* Quad;
	};
}