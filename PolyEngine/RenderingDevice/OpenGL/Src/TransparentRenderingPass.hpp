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
	class TransparentRenderingPass : public RenderingPassBase
	{
	public:
		TransparentRenderingPass(const PostprocessQuad* quad);

	protected:
		void OnRun(World* world, const CameraComponent* camera, const AARect& rect) override;

		const PostprocessQuad* Quad;
	};
}