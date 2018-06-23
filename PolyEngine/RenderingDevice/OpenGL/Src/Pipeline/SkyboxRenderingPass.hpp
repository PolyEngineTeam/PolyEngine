#pragma once

#include "Pipeline/RenderingPassBase.hpp"
#include "Proxy/GLShaderProgram.hpp"
#include "Common/PrimitiveCube.hpp"

namespace Poly
{
	class World;
	class RenderingTargetBase;
	class GLTextureDeviceProxy;
	class AARect;
	class SkyboxWorldComponent;

	class SkyboxRenderingPass : public RenderingPassBase
	{
	public:
		SkyboxRenderingPass(const GLRenderingDevice* rdi);

	protected:
		void OnRun(World* world, const CameraComponent* camera, const AARect& /*rect*/, ePassType passType) override;

		void RenderSkybox(const CameraComponent* camera, const SkyboxWorldComponent* SkyboxWorldCmp);
	};
}