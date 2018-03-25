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
	class SpritesheetRenderingPass : public RenderingPassBase
	{
	public:
		SpritesheetRenderingPass(const PostprocessQuad* quad);

	protected:
		void OnRun(World* world, const CameraComponent* camera, const AARect& rect, ePassType passType) override;

		const PostprocessQuad* Quad;
	};
}