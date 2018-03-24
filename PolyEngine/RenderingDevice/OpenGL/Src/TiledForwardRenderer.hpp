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
		struct PointLight {
			Vector color;
			Vector position;
			Vector paddingAndRadius;
		};

		struct VisibleIndex {
			int index;
		};
		
		const size_t NUM_LIGHTS = 10; // 1024;
		const float LIGHT_RADIUS = 30.0f;

		GLuint SCREEN_SIZE_X = 800;
		GLuint SCREEN_SIZE_Y = 600;
		// X and Y work group dimension variables for compute shader
		GLuint workGroupsX = 0;
		GLuint workGroupsY = 0;
		// Used for storage buffer objects to hold light data and visible light indicies data
		GLuint lightBuffer = 0;
		GLuint visibleLightIndicesBuffer = 0;

		// For drawing our 1 x 1 quad
		GLuint quadVAO = 0;
		GLuint quadVBO;

		void SetupLights();
		void UpdateLights(World* world);
		void DrawQuad();

		inline float Random()
		{
			return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		}

		inline float Random(float min, float max)
		{
			float rnd = Random();
			return Lerp(min, max, rnd);
		}

		inline Vector RandomVector(float min, float max)
		{
			return Vector(Random(min, max), Random(min, max), Random(min, max), 1.0f);
		}
	};
}