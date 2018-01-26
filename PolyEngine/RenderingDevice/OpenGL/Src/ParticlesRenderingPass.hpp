#pragma once

#include "RenderingPassBase.hpp"
#include "GLShaderProgram.hpp"
#include "PostprocessQuad.hpp"

namespace Poly
{
	class World;
	class RenderingTargetBase;
	class GLTextureDeviceProxy;
	class AARect;

	//------------------------------------------------------------------------------
	class ParticlesRenderingPass : public RenderingPassBase
	{
	public:
		ParticlesRenderingPass();

	protected:
		void OnRun(World* world, const CameraComponent* camera, const AARect& rect, ePassType passType) override;

	private:

		unsigned int quadVAO, quadVBO;
		unsigned int instanceVBO;

		static const int instancesLen = 10;
		Dynarray<float> instancesTransform;

		float Random() const;
		float Random(float min, float max) const;
	};
}