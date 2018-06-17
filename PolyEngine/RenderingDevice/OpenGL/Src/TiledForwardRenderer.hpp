#pragma once

#include "IRendererInterface.hpp"
#include "Common/GLUtils.hpp"

namespace Poly {

	class TiledForwardRenderer : public IRendererInterface
	{

	public:
		TiledForwardRenderer(GLRenderingDevice* RenderingDeviceInterface);
		
		void Init() override;
		void Resize(const ScreenSize& size) override;
		void Render(const SceneView& sceneView) override;
		void Deinit() override;

	private:
		// structures defining the data of OpenGL buffers
		struct Light {
			Vector Position;
			Vector Color;
			Vector RangeIntensity;
		};

		struct VisibleIndex {
			int index;
		};

		const size_t MAX_NUM_LIGHTS = 1024;

		// X and Y work group dimension variables for compute shader
		GLuint workGroupsX = 0;
		GLuint workGroupsY = 0;
		
		// Used for storage buffer objects to hold light data and visible light indicies data
		GLuint lightBuffer = 0;
		GLuint visibleLightIndicesBuffer = 0;

		GLShaderProgram depthShader;
		GLShaderProgram lightCullingShader;
		GLShaderProgram LightAccumulationShader;
		GLShaderProgram hdrShader;
		GLShaderProgram SkyboxShader;
		GLShaderProgram SSAOShader;
		GLShaderProgram LinearizeDepthShader;
		GLShaderProgram GammaShader;
		GLShaderProgram ParticleShader;
		GLShaderProgram TranslucentShader;
		GLShaderProgram equiToCubemapShader;
		GLShaderProgram equirectangularToCubemapShader;
		GLShaderProgram cubemapIrradianceShader;
		GLShaderProgram prefilterCubemapShader;
		GLShaderProgram integrateBRDFShader;
		GLShaderProgram text2DShader;
		GLShaderProgram debugLightAccumShader;
		GLShaderProgram debugQuadDepthPrepassShader;

		// Render Targets
		GLuint preDepthBuffer;
		GLuint colorBuffer;
		GLuint normalBuffer;
		GLuint rboDepth;
		GLuint postColorBuffer0;
		GLuint postColorBuffer1;
		GLuint linearDepth;
		
		// IBL textures and cubemaps
		GLuint hdrTexture;
		GLuint envCubemap;
		GLuint irradianceMap;
		GLuint prefilterMap;
		GLuint preintegratedBrdfLUT;

		// Framebufers
		GLuint FBOdepthMap;
		GLuint FBOhdr;
		GLuint FBOpost0;
		GLuint FBOpost1;
		
		void LoadHDR();

		void CaptureCubemap();

		void CaptureDifuseIrradiance();

		void CaptureSpecularPrefilteredMap();

		void CapturePreintegratedBRDF();

		void CreateLightBuffers(const ScreenSize& size);
		
		void DeleteLightBuffers();
		
		void CreateRenderTargets(const ScreenSize& size);

		void DeleteRenderTargets();

		void SetupLightsBufferFromScene();

		void UpdateLightsBufferFromScene(const SceneView& sceneView);

		void RenderDepthPrePass(const SceneView& sceneView);

		void ComputeLightCulling(const SceneView& sceneView);

		void RenderOpaqueLit(const SceneView& sceneView);

		void RenderSkybox(const SceneView& sceneView);

		void RenderEquiCube(const SceneView& sceneView);

		void RenderTranslucentLit(const SceneView& sceneView);
		
		void RenderParticleUnlit(World* world, const CameraComponent* cameraCmp);

		void LinearizeDepth(const SceneView& sceneView);

		void PostTonemapper(const SceneView& sceneView);

		void PostSSAO(const SceneView& sceneView);

		void UIText2D(const SceneView& sceneView);

		void PostGamma();
		
		void DebugDepthPrepass(const SceneView& sceneView);

		void DebugLightAccum(const SceneView& sceneView);
	};
}