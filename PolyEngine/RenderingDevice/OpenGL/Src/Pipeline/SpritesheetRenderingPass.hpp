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
	class SpritesheetRenderingPass : public RenderingPassBase
	{
	public:
		SpritesheetRenderingPass(const GLRenderingDevice* rdi);

	protected:
		void OnRun(World* world, const CameraComponent* camera, const AARect& rect, ePassType passType) override;
	};
}