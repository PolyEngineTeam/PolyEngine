#pragma once

#include <IRenderingDevice.hpp>
#include "GLUtils.hpp"

namespace Poly
{
	class GLTextFieldBufferDeviceProxy : public ITextFieldBufferDeviceProxy
	{
	public:
		GLTextFieldBufferDeviceProxy();
		virtual ~GLTextFieldBufferDeviceProxy();

		void SetContent(size_t count, const TextFieldLetter* letters);

	private:
		GLuint VAO = 0;
		GLuint VBO = 0;
		size_t Size = 0;

		friend class GLRenderingDevice;
	};
}
