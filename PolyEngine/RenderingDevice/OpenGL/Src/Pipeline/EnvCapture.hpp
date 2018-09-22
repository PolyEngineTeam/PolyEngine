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

		// bool GetIsDirty() const { return IsDirty; };

		// GLuint GetCurrentHDRPanorama() const { return HDRPanorama; };
		// GLuint GetCurrentEnvCubemap() const { return EnvCubemap; };
		// GLuint GetCurrentIrradianceMap() const { return IrradianceMap; };
		// GLuint GetCurrentPrefilterMap() const { return PrefilterMap; };

		GLuint GetCurrentHDRPanorama() const { return HDRPanoramas[CurrentEnviroment]; };
		GLuint GetCurrentEnvCubemap() const { return EnvCubemaps[CurrentEnviroment]; };
		GLuint GetCurrentIrradianceMap() const { return IrradianceMaps[CurrentEnviroment]; };
		GLuint GetCurrentPrefilterMap() const { return PrefilterMaps[CurrentEnviroment]; };

	private:

		bool IsDirty = true;

		size_t CurrentEnviroment;

		GLRenderingDevice* RDI;

		// // IBL textures and cubemaps
		// GLuint HDRPanorama = 0;
		// GLuint EnvCubemap = 0;
		// GLuint IrradianceMap = 0;
		// GLuint PrefilterMap = 0;

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