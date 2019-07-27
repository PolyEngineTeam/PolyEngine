#pragma once

#include <Defines.hpp>
#include <Common/GLUtils.hpp>

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

		std::vector<float> InstancesTransform;
	};
}
