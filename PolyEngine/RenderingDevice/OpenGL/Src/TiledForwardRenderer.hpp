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
		GLShaderProgram lightAccumulationShader;
		GLShaderProgram hdrShader;
		GLShaderProgram SkyboxShader;
		GLShaderProgram SSAOShader;
		GLShaderProgram GammaShader;
		GLShaderProgram ParticleShader;
		GLShaderProgram TranslucentShader;

		GLShaderProgram debugQuadDepthPrepassShader;
		GLShaderProgram debugLightAccumShader;

		GLuint preDepthBuffer;
		GLuint colorBuffer;
		GLuint normalBuffer;
		GLuint postColorBuffer0;
		GLuint postColorBuffer1;

		GLuint FBOdepthMap;
		GLuint FBOhdr;
		GLuint FBOpost0;
		GLuint FBOpost1;
		
		void CreateLightBuffers(const GLuint &SCREEN_SIZE_X, const GLuint &SCREEN_SIZE_Y);
		
		void DeleteLightBuffers();
		
		void CreateRenderTargets(const GLuint &SCREEN_SIZE_X, const GLuint &SCREEN_SIZE_Y);

		void SetupLightsBufferFromScene();

		void UpdateLightsBufferFromScene(const SceneView& sceneView);

		void RenderDepthPrePass(const SceneView& sceneView);

		void ComputeLightCulling(const SceneView& sceneView);

		void RenderOpaqueLit(const SceneView& sceneView);

		void RenderSkybox(World* world, const CameraComponent* cameraCmp);

		void RenderTranslucentLit(const SceneView& sceneView);
		
		void RenderParticleUnlit(World* world, const CameraComponent* cameraCmp);

		void PostTonemapper(World* world, const AARect& rect, const CameraComponent* cameraCmp);

		void PostSSAO(const CameraComponent* cameraCmp);

		void PostGamma();
		
		void DebugDepthPrepass(const CameraComponent* cameraCmp);

		void DebugLightAccum(const SceneView& sceneView);
	};
}