#pragma once

#include <map>
#include <Core.hpp>

typedef unsigned int GLuint;
typedef unsigned int GLenum;

namespace Poly {
	class GLShaderProgram : public BaseObject<>
	{
		enum class eShaderUnitType
		{
			VERTEX,
			GEOMETRY,
			FRAGMENT,
			_COUNT
		};

		struct UniformInfo
		{
			UniformInfo() {}
			UniformInfo(const String& name, int location) : TypeName(name), Location(location) {}

			String TypeName;
			int Location = 0;
		};
	public:
		GLShaderProgram(const String& vertex, const String& fragment);
		GLShaderProgram(const String& vertex, const String& geometry, const String& fragment);

		void BindProgram() const;
		void CompileProgram();
		void Validate();

		size_t GetProgramHandle() const;

		
		void SetUniform(const String& name, int val);
		void SetUniform(const String& name, float val);
		void SetUniform(const String& name, float val1, float val2);
		void SetUniform(const String& name, const Vector& val);
		void SetUniform(const String& name, const Color& val);
		void SetUniform(const String& name, const Matrix& val);

	private:
		void LoadShader(eShaderUnitType type, const String &shaderName);

		static GLenum GetEnumFromShaderUnitType(eShaderUnitType type);

		void RegisterUniform(const String& type, const String& name);
		void AnalyzeShaderCode(const String &code);

		std::map<String, UniformInfo> m_uniforms;
		GLuint m_program;
		EnumArray<String, eShaderUnitType> ShaderCode;
	};
}
