#pragma once

#include "Pipeline/RenderingPassBase.hpp"
#include "Common/GLShaderProgram.hpp"
#include "Common/PrimitiveQuad.hpp"

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
		TransparentRenderingPass(const GLRenderingDevice* rdi);

	protected:

		void OnRun(World* world, const CameraComponent* camera, const AARect& rect, ePassType passType) override;
	};
}