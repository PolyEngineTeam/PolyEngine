#pragma once

#include <pe/Defines.hpp>
#include <Common/GLUtils.hpp>
#include <Proxy/GLShaderProgram.hpp>

namespace Poly
{
	class SkyboxWorldComponent;
	class GLRenderingDevice;;
	class GLShaderProgram;

	class EnvCapture : public ::pe::core::BaseObject<>
	{
		// TODO: inherit from multipass RenderingPass, need access to RDI

	public:

		EnvCapture(GLRenderingDevice* rdi);
		~EnvCapture();

		void PrecalculateResourcesIBL(const SkyboxWorldComponent* skyboxCmp);

		GLuint GetCurrentHDRPanorama() const { return HDRPanoramas[CurrentEnviroment]; };
		GLuint GetCurrentEnvCubemap() const { return EnvCubemaps[CurrentEnviroment]; };
		GLuint GetCurrentIrradianceMap() const { return IrradianceMaps[CurrentEnviroment]; };
		GLuint GetCurrentPrefilterMap() const { return PrefilterMaps[CurrentEnviroment]; };

		void SetCurrentEnviroment(size_t value) { CurrentEnviroment = value; };

	private:

		bool IsDirty = true;

		size_t CurrentEnviroment = 0;

		GLRenderingDevice* RDI;

		std::vector<GLuint> HDRPanoramas;
		std::vector<GLuint> EnvCubemaps;
		std::vector<GLuint> IrradianceMaps;
		std::vector<GLuint> PrefilterMaps;

		GLShaderProgram EquirectangularToCubemapShader;
		GLShaderProgram CubemapIrradianceShader;
		GLShaderProgram PrefilterCubemapShader;

		GLuint CaptureCubemap(const GLuint equiPanorama);

		GLuint CaptureDiffuseIrradiance(const GLuint envCubemap);

		GLuint CaptureSpecularPrefilteredMap(const GLuint envCubemap);
	};
}