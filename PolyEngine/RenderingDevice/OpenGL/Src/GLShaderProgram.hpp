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
			UniformInfo(const String& type, int location) : TypeName(type), Location(location) {}

			String TypeName;
			int Location = 0;
		};

		struct OutputInfo
		{
			OutputInfo() {}
			OutputInfo(const String& type, size_t index) : TypeName(type), Index(index) {}

			String TypeName;
			size_t Index = 0;
		};
	public:
		GLShaderProgram(const String& vertex, const String& fragment);
		GLShaderProgram(const String& vertex, const String& geometry, const String& fragment);

		void BindProgram() const;

		size_t GetProgramHandle() const;

		void SetUniform(const String& name, int val);
		void SetUniform(const String& name, float val);
		void SetUniform(const String& name, float val1, float val2);
		void SetUniform(const String& name, const Vector& val);
		void SetUniform(const String& name, const Color& val);
		void SetUniform(const String& name, const Matrix& val);

		const std::map<String, OutputInfo>& GetOutputsInfo() const { return Outputs; }
		const std::map<String, UniformInfo>& GetUniformsInfo() const { return Uniforms; }

		void RegisterUniform(const String& type, const String& name);
	private:
		void CompileProgram();
		void Validate();
		void LoadShader(eShaderUnitType type, const String &shaderName);

		static GLenum GetEnumFromShaderUnitType(eShaderUnitType type);

		void AnalyzeShaderCode(eShaderUnitType type);

		std::map<String, UniformInfo> Uniforms;
		std::map<String, OutputInfo> Outputs;
		GLuint ProgramHandle;
		EnumArray<String, eShaderUnitType> ShaderCode;

		String VertexProgramPath;
		String GeometryProgramPath;
		String FragmentProgramPath;
	};
}
