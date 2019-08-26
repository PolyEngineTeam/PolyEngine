#pragma once

#include <pe/Defines.hpp>
#include <Common/GLUtils.hpp>

namespace Poly
{
	class GLMeshDeviceProxy : public IMeshDeviceProxy
	{
	private:
		enum class eBufferType {
			VERTEX_BUFFER,
			TEXCOORD_BUFFER,
			NORMAL_BUFFER,
			TANGENT_BUFFER,
			BITANGENT_BUFFER,
			INDEX_BUFFER,
			BONE_INDEX_BUFFER,
			BONE_WEIGHT_BUFFER,
			_COUNT
		};

	public:
		GLMeshDeviceProxy();
		virtual ~GLMeshDeviceProxy();

		void SetContent(const Mesh& mesh);
		unsigned int GetResourceID() const { return VAO; };

		GLuint GetVAO() const { return VAO; }

	private:
		void EnsureVBOCreated(eBufferType type);

		GLuint VAO = 0;
		::pe::core::utils::EnumArray<GLuint, eBufferType> VBO;
	};
}