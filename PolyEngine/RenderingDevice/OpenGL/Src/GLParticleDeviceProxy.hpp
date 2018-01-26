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
			VERTEX_BUFFER,
			TEXCOORD_BUFFER,
			INSTANCE,
			_COUNT
		};

	public:
		GLParticleDeviceProxy();
		virtual ~GLParticleDeviceProxy();

		void SetContent(const ParticleEmitter& particles);

		GLuint GetVAO() const { return quadVAO; }

	private:
		void EnsureVBOCreated(eBufferType type);

		// GLuint VAO = 0;
		// EnumArray<GLuint, eBufferType> VBO;

		unsigned int quadVAO, quadVBO;
		unsigned int instanceVBO;

		static const int instancesLen = 10;
		Dynarray<float> instancesTransform;

		friend class GLRenderingDevice;
	};
}
