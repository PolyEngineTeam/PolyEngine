#pragma once

#include <Defines.hpp>
#include <Common/GLUtils.hpp>
#include <Proxy/GLShaderProgram.hpp>

namespace Poly
{
	class RenderingSettingsComponent;
	class GLRenderingDevice;
	class GLShaderProgram;
	struct SceneView;

	Matrix GetProjectionForShadowMap(const SceneView& sceneView, int shadowmapSize);

	void StablizeShadowProjection(Poly::Matrix& clipFromWorld, int shadowmapSize);
	
	int GetShadowMapSize(const RenderingSettingsComponent* renderingSettingsCmp);


	class ShadowMapPass : public BaseObject<>
	{
	public:

		ShadowMapPass(GLRenderingDevice* rdi);
		~ShadowMapPass();

		void Init(const SceneView& sceneView);
		void Render(const SceneView& sceneView);
		void Deinit();

		GLuint GetDirShadowMapColor() const { return DirShadowMapColor; };
		GLuint GetEVSMap0() const { return EVSMap0; };

	private:

		GLRenderingDevice* RDI;

		GLuint DirShadowMapDepth;
		GLuint DirShadowMapColor;
		GLuint EVSMap0;
		GLuint EVSMap1;
		GLuint FBOShadowDepthMap;
		GLuint FBOShadowMapResolve0;
		GLuint FBOShadowMapResolve1;
		
		GLShaderProgram ShadowMapShader;
		GLShaderProgram EVSMResolveShader;
		GLShaderProgram EVSMBlurShader;

		void RenderEVSM(const SceneView& sceneView);
		void RenderPCF(const SceneView& sceneView);
	};
}