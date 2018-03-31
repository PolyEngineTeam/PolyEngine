#pragma once

#include <Rendering/IRenderingDevice.hpp>
#include "Common/GLUtils.hpp"

namespace Poly
{
	class GLTextFieldBufferDeviceProxy : public ITextFieldBufferDeviceProxy
	{
	public:
		GLTextFieldBufferDeviceProxy();
		virtual ~GLTextFieldBufferDeviceProxy();

		void SetContent(size_t count, const TextFieldLetter* letters);

		GLuint GetVAO() const { return VAO; }
		size_t GetSize() const { return Size; }
	private:
		GLuint VAO = 0;
		GLuint VBO = 0;
		size_t Size = 0;

		friend class GLRenderingDevice;
	};
}
