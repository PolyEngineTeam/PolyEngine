#pragma once

#include "IRendererInterface.hpp"
#include "GLUtils.hpp"

namespace Poly {

	class TiledForwardRenderer : public IRendererInterface
	{

	public:
		TiledForwardRenderer(GLRenderingDevice* RenderingDeviceInterface);

		void Init() override;
		void Render(World* world, const AARect& rect, const CameraComponent* cameraCmp) override;
		void Deinit() override;

	private:
		// structures defining the data of both buffers
		struct Light {
			Vector Position;
			Vector Color;
			Vector RangeIntensity;
		};

		struct VisibleIndex {
			int index;
		};

		const size_t MAX_NUM_LIGHTS = 1024;

		GLuint SCREEN_SIZE_X = 1920; // 800; // 1280
		GLuint SCREEN_SIZE_Y = 1080; // 600; // 720
		// X and Y work group dimension variables for compute shader
		GLuint workGroupsX = 0;
		GLuint workGroupsY = 0;
		// Used for storage buffer objects to hold light data and visible light indicies data
		GLuint lightBuffer = 0;
		GLuint visibleLightIndicesBuffer = 0;

		// For drawing our 1 x 1 quad
		GLuint quadVAO = 0;
		GLuint quadVBO = 0;

		GLShaderProgram depthShader;
		GLShaderProgram lightCullingShader;
		GLShaderProgram lightAccumulationShader;
		GLShaderProgram hdrShader;

		GLShaderProgram debugQuadDepthPrepassShader;
		GLShaderProgram debugLightAccumShader;

		GLuint depthMap;
		GLuint colorBuffer;

		GLuint FBOdepthMap;
		GLuint FBOhdr;

		GLuint FallbackWhiteTexture;
		GLuint FallbackNormalMap;

		float exposure = 1.0f;

		int DynamicLighsInFrame = 0;

		void SetupLightsBufferFromScene();

		void UpdateLightsBufferFromScene(World* world);

		void DepthPrePass(World* world, const CameraComponent* cameraCmp);

		void LightCulling(World* world, const CameraComponent* cameraCmp);

		void DrawDepthPrepass(const CameraComponent* cameraCmp);

		void DrawLightAccum(World* world, const CameraComponent* cameraCmp);

		void AccumulateLights(World* world, const CameraComponent* cameraCmp);

		void Tonemapper(World* world, const CameraComponent* cameraCmp);
		
		void DrawQuad();

		void CreateFallbackTextures();
	};
}