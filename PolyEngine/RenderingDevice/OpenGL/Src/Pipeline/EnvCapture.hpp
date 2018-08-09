#pragma once

#include <Defines.hpp>
#include "Common/GLUtils.hpp"
#include "Proxy/GLShaderProgram.hpp"

namespace Poly
{
	class SkyboxWorldComponent;
	class GLRenderingDevice;;
	class GLShaderProgram;

	class EnvCapture : public BaseObject<>
	{
		// TODO: inherit from multipass RenderingPass, need access to RDI

	public:

		EnvCapture(GLRenderingDevice* rdi);
		~EnvCapture();

		void UpdateEnv(const SkyboxWorldComponent* skyboxCmp);

		bool GetIsDirty() const { return IsDirty; };

		GLuint GetHDRPanorama() const { return HDRPanorama; };
		GLuint GetEnvCubemap() const { return EnvCubemap; };
		GLuint GetIrradianceMap() const { return IrradianceMap; };
		GLuint GetPrefilterMap() const { return PrefilterMap; };

	private:

		bool IsDirty = true;

		GLRenderingDevice* RDI;

		// IBL textures and cubemaps
		GLuint HDRPanorama = 0;
		GLuint EnvCubemap = 0;
		GLuint IrradianceMap = 0;
		GLuint PrefilterMap = 0;

		GLShaderProgram EquirectangularToCubemapShader;
		GLShaderProgram CubemapIrradianceShader;
		GLShaderProgram PrefilterCubemapShader;

		void CaptureCubemap(const SkyboxWorldComponent* skyboxCmp);

		void CaptureDiffuseIrradiance();

		void CaptureSpecularPrefilteredMap();
	};
}