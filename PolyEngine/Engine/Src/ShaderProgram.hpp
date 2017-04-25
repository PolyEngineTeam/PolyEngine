#pragma once

#include <map>
#include <Core.hpp>

namespace Poly {
	class ShaderProgram : public BaseObject<>
	{
	public:
		ShaderProgram(const String& vertex, const String& fragment);
		
		void BindProgram() const;
		void CompileProgram();
		void Validate();
		void LoadShader(int type, const String &shaderName);

		size_t GetProgramHandle() const;

		void RegisterUniform(const String &name);
		void SetUniform(const String& name, int val);
		void SetUniform(const String& name, float val);
		void SetUniform(const String& name, const Vector& val);
		void SetUniform(const String& name, const Matrix& val);

	private:
		std::map<String, int> m_uniforms;
		size_t m_program;
	};
}