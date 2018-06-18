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
	class PostprocessRenderingPass : public RenderingPassBase
	{
	public:
		PostprocessRenderingPass(const GLRenderingDevice* rdi, const String& fragment);
		PostprocessRenderingPass(const GLRenderingDevice* rdi, const String& geometry, const String& fragment);
	
	protected:
		virtual void OnRun(World* world, const CameraComponent* camera, const AARect& rect, ePassType passType) override;
	};
}