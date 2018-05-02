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
			float Radius;
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

		GLuint depthMapFBO;
		GLuint depthMap;

		GLuint FallbackWhiteTexture;

		int DynamicLighsInFrame = 0;

		// Dynarray<Vector> LightsStartPositions;

		// void SetupLightsBuffer();

		void SetupLightsBufferFromScene();

		// void UpdateLightsBuffer(World* world);

		void UpdateLightsBufferFromScene(World* world);

		void DepthPrePass(World* world, const CameraComponent* cameraCmp);

		void LightCulling(World* world, const CameraComponent* cameraCmp);

		// void DebugLightCulling(World * world, const CameraComponent * cameraCmp);

		void DrawDepthPrepass(const CameraComponent* cameraCmp);

		// void DrawLightCulling(const CameraComponent* cameraCmp);

		// void DebugDepth(World* world, const CameraComponent* cameraCmp);

		void DrawLightAccum(World* world, const CameraComponent* cameraCmp);
		
		void DrawQuad();

		void CreateFallbackWhiteTexture();

		// inline float Random()
		// {
		// 	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		// }

		// inline float Random(float min, float max)
		// {
		// 	float rnd = Random();
		// 	return Lerp(min, max, rnd);
		// }

		// inline Vector RandomVector(float min, float max)
		// {
		// 	return Vector(Random(min, max), Random(min, max), Random(min, max), 1.0f);
		// }
	};
}