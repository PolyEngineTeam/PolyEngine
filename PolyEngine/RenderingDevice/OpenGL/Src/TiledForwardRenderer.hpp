#pragma once

#include "IRendererInterface.hpp"
#include "Common/GLUtils.hpp"

namespace Poly {

	class GLShaderProgram;

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
			int Index;
		};

		const size_t MAX_NUM_LIGHTS = 1024;
		const int MAX_LIGHT_COUNT_DIRECTIONAL = 8;

		// X and Y work group dimension variables for compute shader
		GLuint WorkGroupsX = 0;
		GLuint WorkGroupsY = 0;
		
		// Used for storage buffer objects to hold light data and visible light indicies data
		GLuint LightBuffer = 0;
		GLuint VisibleLightIndicesBuffer = 0;

		GLShaderProgram DepthShader;
		GLShaderProgram LightCullingShader;
		GLShaderProgram LightAccumulationShader;
		GLShaderProgram HDRShader;
		GLShaderProgram SkyboxShader;
		GLShaderProgram SSAOShader;
		GLShaderProgram LinearizeDepthShader;
		GLShaderProgram GammaShader;
		GLShaderProgram ParticleShader;
		GLShaderProgram TranslucentShader;
		GLShaderProgram EquiToCubemapShader;
		GLShaderProgram EquirectangularToCubemapShader;
		GLShaderProgram CubemapIrradianceShader;
		GLShaderProgram PrefilterCubemapShader;
		GLShaderProgram IntegrateBRDFShader;
		GLShaderProgram Text2DShader;
		GLShaderProgram EditorDebugShader;
		GLShaderProgram DebugLightAccumShader;
		GLShaderProgram DebugQuadDepthPrepassShader;

		// Render Targets
		GLuint PreDepthBuffer;
		GLuint ColorBuffer;
		GLuint NormalBuffer;
		GLuint RboDepth;
		GLuint PostColorBuffer0;
		GLuint PostColorBuffer1;
		GLuint LinearDepth;
		
		// IBL textures and cubemaps
		GLuint HDRTexture;
		GLuint EnvCubemap;
		GLuint IrradianceMap;
		GLuint PrefilterMap;
		GLuint PreintegratedBrdfLUT;

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

		void EditorDebug(const SceneView& sceneView);

		void UIText2D(const SceneView& sceneView);

		void PostGamma();
		
		void DebugDepthPrepass(const SceneView& sceneView);

		void DebugLightAccum(const SceneView& sceneView);
	};
}