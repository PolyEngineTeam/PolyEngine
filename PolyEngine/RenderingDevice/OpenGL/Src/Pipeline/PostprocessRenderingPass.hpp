#pragma once

#include <pe/Defines.hpp>
#include <Pipeline/RenderingPassBase.hpp>
#include <Proxy/GLShaderProgram.hpp>
#include <Common/PrimitiveQuad.hpp>

namespace pe::core::math
{
	class AARect;
}

namespace Poly
{
	class Scene;
	class RenderingTargetBase;
	class GLTextureDeviceProxy;

	//------------------------------------------------------------------------------
	class PostprocessRenderingPass : public RenderingPassBase
	{
	public:
		PostprocessRenderingPass(const GLRenderingDevice* rdi, const core::storage::String& fragment);
		PostprocessRenderingPass(const GLRenderingDevice* rdi, const core::storage::String& geometry, const core::storage::String& fragment);
	
	protected:
		virtual void OnRun(Scene* world, const CameraComponent* camera, const core::math::AARect& rect, ePassType passType) override;
	};
}