#pragma once

#include <map>
#include <Core.hpp>

typedef unsigned int GLuint;
typedef unsigned int GLenum;

namespace Poly {
	class GLShaderProgram : public BaseObject<>
	{
	public:
		GLShaderProgram(const String& vertex, const String& fragment);
		GLShaderProgram(const String& vertex, const String& geometry, const String& fragment);

		void BindProgram() const;
		void CompileProgram();
		void Validate();
		void LoadShader(GLenum type, const String &shaderName);

		size_t GetProgramHandle() const;

		void RegisterUniform(const String &name);
		void SetUniform(const String& name, int val);
		void SetUniform(const String& name, float val);
		void SetUniform(const String& name, float val1, float val2);
		void SetUniform(const String& name, const Vector& val);
		void SetUniform(const String& name, const Color& val);
		void SetUniform(const String& name, const Matrix& val);

	private:
		void AnalyzeShaderCode(const String &code);

		std::map<String, int> m_uniforms;
		GLuint m_program;
	};
}
