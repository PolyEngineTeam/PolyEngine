#pragma once

#include <Defines.hpp>
#include <Common/GLUtils.hpp>
#include <Proxy/GLShaderProgram.hpp>

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

		void PrecalculateResourcesIBL(const SkyboxWorldComponent* skyboxCmp);

		GLuint GetCurrentHDRPanorama() const { return HDRPanoramas[CurrentEnviroment]; };
		GLuint GetCurrentEnvCubemap() const { return EnvCubemaps[CurrentEnviroment]; };
		GLuint GetCurrentIrradianceMap() const { return IrradianceMaps[CurrentEnviroment]; };
		GLuint GetCurrentPrefilterMap() const { return PrefilterMaps[CurrentEnviroment]; };

		void SetCurrentEnviroment(size_t value) { ASSERTE(value >= 0, "Wrong CurrentEnviroment index"); CurrentEnviroment = value; };

	private:

		bool IsDirty = true;

		size_t CurrentEnviroment = 0;

		GLRenderingDevice* RDI;

		Dynarray<GLuint> HDRPanoramas;
		Dynarray<GLuint> EnvCubemaps;
		Dynarray<GLuint> IrradianceMaps;
		Dynarray<GLuint> PrefilterMaps;

		GLShaderProgram EquirectangularToCubemapShader;
		GLShaderProgram CubemapIrradianceShader;
		GLShaderProgram PrefilterCubemapShader;

		GLuint CaptureCubemap(const GLuint equiPanorama);

		GLuint CaptureDiffuseIrradiance(const GLuint envCubemap);

		GLuint CaptureSpecularPrefilteredMap(const GLuint envCubemap);
	};
}