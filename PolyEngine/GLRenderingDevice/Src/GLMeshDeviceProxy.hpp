#pragma once

#include <IRenderingDevice.hpp>
#include <EnumUtils.hpp>
#include "GLUtils.hpp"

namespace Poly
{
	class GLMeshDeviceProxy : public IMeshDeviceProxy
	{
	private:
		enum class eBufferType {
			VERTEX_BUFFER,
			TEXCOORD_BUFFER,
			NORMAL_BUFFER,
			INDEX_BUFFER,
			_COUNT
		};

	public:
		GLMeshDeviceProxy();
		virtual ~GLMeshDeviceProxy();

		void SetContent(const Mesh& mesh);

	private:
		void EnsureVBOCreated(eBufferType type);

		GLuint VAO = 0;
		EnumArray<GLuint, eBufferType> VBO;

		friend class GLRenderingDevice;
	};
}
