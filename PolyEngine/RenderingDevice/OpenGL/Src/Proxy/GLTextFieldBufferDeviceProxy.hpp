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

		void SetContent(size_t count, const TextFieldLetter* letters) override;
		virtual unsigned int GetResourceID() const override { return (unsigned int)VAO; }
		virtual unsigned int GetResourceSize() const override { return (unsigned int)Size; }

		GLuint GetVAO() const { return VAO; }
		size_t GetSize() const { return Size; }

	private:
		GLuint VAO = 0;
		GLuint VBO = 0;
		size_t Size = 0;
	};
}
