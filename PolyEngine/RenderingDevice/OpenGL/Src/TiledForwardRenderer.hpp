#pragma once

#include "IRendererInterface.hpp"
#include "GLUtils.hpp"
#include "GLShaderProgram.hpp"

namespace Poly {

	class GLShaderProgram;

	class TiledForwardRenderer : public IRendererInterface
	{

	public:
		struct PointLight {
			Vector color;
			Vector position;
			Vector paddingAndRadius;
		};

		struct VisibleIndex {
			int index;
		};

	public:
		TiledForwardRenderer(GLRenderingDevice* RenderingDeviceInterface);

		void Init() override;
		void Render(World* world, const AARect& rect, CameraComponent* cameraCmp) override;
		void Deinit() override;

	private:
		void RenderLit(World* world, const AARect& rect, CameraComponent* cameraCmp);

		void DrawMeshes(World* world, GLShaderProgram& shader, const Matrix& mvp);

		void UpdateLights(World* world) const;

		void DrawQuad();
		
		void CreateDummyTexture();

		// const glm::ivec2 SCREEN_SIZE(1920 / 2, 1080 / 2);
		const unsigned int SCREEN_SIZE_X = 800;
		const unsigned int SCREEN_SIZE_Y = 600;
		const unsigned int NUM_LIGHTS = 1024;
		
		GLuint workGroupsX = 0;
		GLuint workGroupsY = 0;

		GLuint lightBuffer = 0;
		GLuint visibleLightIndicesBuffer = 0;

		GLuint quadVAO = 0;
		GLuint quadVBO = 0;

		GLShaderProgram depthShader;
		GLShaderProgram lightCullingShader;
		GLShaderProgram lightAccumulationShader;
		GLShaderProgram hdrShader;
		GLShaderProgram depthDebugShader;
		GLShaderProgram lightDebugShader;
		
		GLuint depthMapFBO;
		GLuint depthMap;

		GLuint hdrFBO;
		GLuint colorBuffer;
		GLuint rboDepth;

		GLuint WhiteDummy;
	};
}