#pragma once

#include <Defines.hpp>
#include <Pipeline/RenderingPassBase.hpp>
#include <Proxy/GLShaderProgram.hpp>
#include <Common/PrimitiveCube.hpp>

namespace Poly
{
	class Scene;
	class RenderingTargetBase;
	class GLTextureDeviceProxy;
	class AARect;
	class SkyboxWorldComponent;

	class SkyboxRenderingPass : public RenderingPassBase
	{
	public:
		SkyboxRenderingPass(const GLRenderingDevice* rdi);

	protected:
		void OnRun(Scene* world, const CameraComponent* camera, const AARect& /*rect*/, ePassType passType) override;

		void RenderSkybox(const CameraComponent* camera, const SkyboxWorldComponent* SkyboxWorldCmp);
	};
}