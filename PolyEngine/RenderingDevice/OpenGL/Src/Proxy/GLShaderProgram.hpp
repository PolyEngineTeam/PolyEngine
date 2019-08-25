#pragma once

#include <pe/Defines.hpp>

typedef unsigned int GLuint;
typedef unsigned int GLenum;

namespace Poly
{
	class GLShaderProgram : public ::pe::core::BaseObject<>
	{
		enum class eShaderUnitType
		{
			VERTEX,
			GEOMETRY,
			FRAGMENT,
			COMPUTE,
			_COUNT
		};

		struct UniformInfo
		{
			UniformInfo() {}
			UniformInfo(const core::storage::String& type, int location) : TypeName(type), Location(location) {}

			core::storage::String TypeName;
			int Location = 0;
		};

		struct OutputInfo
		{
			OutputInfo() {}
			OutputInfo(const core::storage::String& type, size_t index) : TypeName(type), Index(index) {}

			core::storage::String TypeName;
			size_t Index = 0;
		};

	public:
		GLShaderProgram(const core::storage::String& compute);
		GLShaderProgram(const core::storage::String& vertex, const core::storage::String& fragment);
		GLShaderProgram(const core::storage::String& vertex, const core::storage::String& geometry, const core::storage::String& fragment);

		void BindProgram() const;

		unsigned int GetProgramHandle() const;

		void SetUniform(const core::storage::String& name, int val);
		void SetUniform(const core::storage::String& name, uint val);
		void SetUniform(const core::storage::String& name, float val);
		void SetUniform(const core::storage::String& name, float val1, float val2);
		void SetUniform(const core::storage::String& name, const core::math::Vector& val);
		void SetUniform(const core::storage::String& name, const core::math::Color& val);
		void SetUniform(const core::storage::String& name, const core::math::Matrix& val);
		void BindSampler(const core::storage::String& name, int samplerID, int textureID);
		void BindSamplerCube(const core::storage::String& name, int samplerID, int cubemapID);

		const std::unordered_map<core::storage::String, OutputInfo>& GetOutputsInfo() const { return Outputs; }
		const std::unordered_map<core::storage::String, UniformInfo>& GetUniformsInfo() const { return Uniforms; }

		void RegisterUniform(const core::storage::String& type, const core::storage::String& name);

	private:

		void LoadShader(eShaderUnitType type, const core::storage::String &shaderName);
		void CompileShader(GLShaderProgram::eShaderUnitType type);
		void Validate();
		void CompileProgram();

		static GLenum GetEnumFromShaderUnitType(eShaderUnitType type);

		void AnalyzeShaderCode(eShaderUnitType type);

		std::unordered_map<core::storage::String, UniformInfo> Uniforms;
		std::unordered_map<core::storage::String, OutputInfo> Outputs;
		GLuint ProgramHandle;
		core::utils::EnumArray<core::storage::String, eShaderUnitType> ShaderCode;

		core::storage::String VertexProgramPath;
		core::storage::String GeometryProgramPath;
		core::storage::String FragmentProgramPath;
	};
}
