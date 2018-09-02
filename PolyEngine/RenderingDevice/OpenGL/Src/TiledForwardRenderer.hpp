#pragma once

#include "IRendererInterface.hpp"
#include "Proxy/GLShaderProgram.hpp"
#include "Common/GLUtils.hpp"
#include "Pipeline/EnvCapture.hpp"

namespace Poly {

	class RenderTargetPingPong : public BaseObject<>
	{
	public:
		void Init(int width, int height);
		void Deinit();

		GLuint GetWriteFBO() const { return pingpongFBO[Uses % 2]; };
		GLuint GetReadFBO() const { return pingpongFBO[(Uses + 1) % 2]; };

		GLuint GetWriteTarget() const { return pingpongTarget[Uses % 2]; };
		GLuint GetReadTarget() const { return pingpongTarget[(Uses + 1) % 2]; };

		void Flip()
		{
			// gConsole.LogInfo("RenderTargetPingPong::Flip Uses: {}, Write: {}, Read: {}",
			//	Uses, (Uses % 2), (Uses + 1) % 2);

			Uses++;
		};

	private:
		GLuint pingpongFBO[2];
		GLuint pingpongTarget[2];

		int Uses = 0;
		int Width = 0;
		int Height = 0;
	};

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
		struct Light
		{
			Vector Position;
			Vector Color;
			Vector RangeIntensity;
		};

		struct VisibleIndex
		{
			int Index;
		};

		Matrix PreviousFrameCameraTransform;
		Matrix PreviousFrameCameraClipFromWorld;

		const int MAX_NUM_LIGHTS = 1024;
		const int MAX_LIGHT_COUNT_DIRECTIONAL = 8;

		const unsigned int SHADOW_WIDTH = 1024;
		const unsigned int SHADOW_HEIGHT = 1024;

		// X and Y work group dimension variables for compute shader
		GLuint WorkGroupsX = 0;
		GLuint WorkGroupsY = 0;

		// Used for storage buffer objects to hold light data and visible light indicies data
		GLuint LightBuffer = 0;
		GLuint VisibleLightIndicesBuffer = 0;

		// Render Targets
		GLuint PreDepthBuffer;
		GLuint ColorBuffer;
		GLuint NormalBuffer;
		GLuint RboDepth;
		GLuint PostColorBuffer0;
		GLuint PostColorBuffer1;
		GLuint PostColorBufferHalfRes;
		GLuint LinearDepth;
		GLuint DirShadowMap;

		// IBL textures and cubemaps
		GLuint PreintegratedBrdfLUT;

		// Framebufers
		GLuint FBOdepthMap;
		GLuint FBOhdr;
		GLuint FBOpost0;
		GLuint FBOpost1;
		GLuint FBOShadowDepthMap;

		// Render pass for IBL environment
		EnvCapture SkyboxCapture;
		RenderTargetPingPong RTBloom;
		TextureResource* Splash;

		// Shader programs
		GLShaderProgram ShadowMapShader;
		GLShaderProgram DepthShader;
		GLShaderProgram LightCullingShader;
		GLShaderProgram LightAccumulationShader;
		GLShaderProgram HDRShader;
		GLShaderProgram SkyboxShader;
		GLShaderProgram LinearizeDepthShader;
		GLShaderProgram GammaShader;
		GLShaderProgram MotionBlurShader;
		GLShaderProgram DOFBokehShader;
		GLShaderProgram DOFApplyShader;
		GLShaderProgram BloomBrightShader;
		GLShaderProgram BloomBlurShader;
		GLShaderProgram BloomApplyShader;
		GLShaderProgram ParticleShader;
		GLShaderProgram TranslucentShader;
		GLShaderProgram EquiToCubemapShader;
		GLShaderProgram IntegrateBRDFShader;
		GLShaderProgram Text2DShader;
		GLShaderProgram EditorDebugShader;
		GLShaderProgram DebugQuadDepthPrepassShader;
		GLShaderProgram DebugQuadLightCullingShader;
		GLShaderProgram DebugLightAccumShader;
		GLShaderProgram DebugTextureInputsShader;

		void CapturePreintegratedBRDF();

		void CreateLightBuffers(const ScreenSize& size);
		
		void DeleteLightBuffers();
		
		void CreateRenderTargets(const ScreenSize& size);

		void DeleteRenderTargets();

		void SetupLightsBufferFromScene();

		void UpdateLightsBufferFromScene(const SceneView& sceneView);

		void UpdateEnvCapture(const SceneView& sceneView);

		void RenderDepthPrePass(const SceneView& sceneView);

		void ComputeLightCulling(const SceneView& sceneView);

		void RenderOpaqueLit(const SceneView& sceneView);

		void RenderSkybox(const SceneView& sceneView);
		
		void RenderShadowMap(const SceneView& sceneView);

		void RenderEquiCube(const SceneView& sceneView);

		void RenderTranslucentLit(const SceneView& sceneView);
		
		void RenderParticleUnlit(Scene* world, const CameraComponent* cameraCmp);

		void LinearizeDepth(const SceneView& sceneView);

		void PostDepthOfField(const SceneView& sceneView);

		void PostMotionBlur(const SceneView& sceneView);

		void PostBloom(const SceneView& sceneView);

		void PostTonemapper(const SceneView& sceneView);

		void EditorDebug(const SceneView& sceneView);

		void UIText2D(const SceneView& sceneView);

		void PostGamma(const SceneView& sceneView);
		
		void DebugDepthPrepass(const SceneView& sceneView);

		void DebugLightAccum(const SceneView& sceneView);
	};
}