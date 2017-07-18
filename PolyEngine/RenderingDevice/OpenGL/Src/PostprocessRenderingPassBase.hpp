#pragma once

#include "RenderingPassBase.hpp"
#include "GLShaderProgram.hpp"

namespace Poly
{
	class World;
	class RenderingTargetBase;
	class GLTextureDeviceProxy;
	class AABox;

	//------------------------------------------------------------------------------
	class PostprocessRenderingPassBase : public RenderingPassBase
	{
	public:
		PostprocessRenderingPassBase(const String& fragment);
		PostprocessRenderingPassBase(const String& geometry, const String& fragment);
	
	protected:
		virtual void OnRun(World* world, const CameraComponent* camera, const AABox& rect) override;
	};
}