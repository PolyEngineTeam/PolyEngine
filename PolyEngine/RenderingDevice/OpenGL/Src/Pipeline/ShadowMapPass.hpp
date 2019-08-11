#pragma once

#include <pe/Defines.hpp>
#include <Common/GLUtils.hpp>
#include <Proxy/GLShaderProgram.hpp>
#include <Rendering/RenderingSettingsComponent.hpp>

namespace Poly
{
	class GLRenderingDevice;
	class GLShaderProgram;
	struct SceneView;

	Matrix GetProjectionForShadowMap(const SceneView& sceneView, int shadowmapSize);

	void StablizeShadowProjection(Poly::Matrix& clipFromWorld, int shadowmapSize);
	
	int GetShadowMapSize(const eShadowMapSize shadowMapSize);


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
		int GetShadowMapResolution() const { return ShadowMapResolution; };
		eShadowType GetShadowType() const { return ShadowType; };

	private:

		GLRenderingDevice* RDI;

		int ShadowMapResolution;
		eShadowType ShadowType;

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