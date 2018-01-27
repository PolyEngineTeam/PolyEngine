#pragma once

#include <IRenderingDevice.hpp>
#include <EnumUtils.hpp>
#include "GLUtils.hpp"

namespace Poly
{
	class GLParticleDeviceProxy : public IParticleDeviceProxy
	{
	private:
		enum class eBufferType {
			VERTEX_AND_UV_BUFFER,
			INSTANCE_BUFFER,
			_COUNT
		};

	public:
		GLParticleDeviceProxy();
		virtual ~GLParticleDeviceProxy();

		void SetContent(const ParticleEmitter& particles);

		GLuint GetVAO() const { return VAO; }

	private:

		GLuint VAO = 0;
		GLuint VBO = 0;
		unsigned int instanceVBO;

		static const int instancesLen = 10;
		Dynarray<float> instancesTransform;

		float Random() const;
		float Random(float min, float max) const;

		friend class GLRenderingDevice;
	};
}
