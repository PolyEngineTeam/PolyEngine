#pragma once

#include <map>
#include <Core.hpp>

typedef unsigned int GLuint;
typedef unsigned int GLenum;

namespace Poly {
	class ShaderProgram : public BaseObject<>
	{
	public:
		ShaderProgram(const String& vertex, const String& fragment);
		ShaderProgram(const String& vertex, const String& geometry, const String& fragment);

		void BindProgram() const;
		void CompileProgram();
		void Validate();
		void LoadShader(GLenum type, const String &shaderName);

		size_t GetProgramHandle() const;

		void RegisterUniform(const String &name);
		void SetUniform(const String& name, int val);
		void SetUniform(const String& name, float val);
		void SetUniform(const String& name, const Vector& val);
		void SetUniform(const String& name, const Matrix& val);

	private:
		std::map<String, int> m_uniforms;
		GLuint m_program;
	};
}
