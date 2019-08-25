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
			UniformInfo(const ::pe::core::storage::String& type, int location) : TypeName(type), Location(location) {}

			::pe::core::storage::String TypeName;
			int Location = 0;
		};

		struct OutputInfo
		{
			OutputInfo() {}
			OutputInfo(const ::pe::core::storage::String& type, size_t index) : TypeName(type), Index(index) {}

			::pe::core::storage::String TypeName;
			size_t Index = 0;
		};

	public:
		GLShaderProgram(const ::pe::core::storage::String& compute);
		GLShaderProgram(const ::pe::core::storage::String& vertex, const ::pe::core::storage::String& fragment);
		GLShaderProgram(const ::pe::core::storage::String& vertex, const ::pe::core::storage::String& geometry, const ::pe::core::storage::String& fragment);

		void BindProgram() const;

		unsigned int GetProgramHandle() const;

		void SetUniform(const ::pe::core::storage::String& name, int val);
		void SetUniform(const ::pe::core::storage::String& name, uint val);
		void SetUniform(const ::pe::core::storage::String& name, float val);
		void SetUniform(const ::pe::core::storage::String& name, float val1, float val2);
		void SetUniform(const ::pe::core::storage::String& name, const ::pe::core::math::Vector& val);
		void SetUniform(const ::pe::core::storage::String& name, const ::pe::core::math::Color& val);
		void SetUniform(const ::pe::core::storage::String& name, const ::pe::core::math::Matrix& val);
		void BindSampler(const ::pe::core::storage::String& name, int samplerID, int textureID);
		void BindSamplerCube(const ::pe::core::storage::String& name, int samplerID, int cubemapID);

		const std::unordered_map<::pe::core::storage::String, OutputInfo>& GetOutputsInfo() const { return Outputs; }
		const std::unordered_map<::pe::core::storage::String, UniformInfo>& GetUniformsInfo() const { return Uniforms; }

		void RegisterUniform(const ::pe::core::storage::String& type, const ::pe::core::storage::String& name);

	private:

		void LoadShader(eShaderUnitType type, const ::pe::core::storage::String &shaderName);
		void CompileShader(GLShaderProgram::eShaderUnitType type);
		void Validate();
		void CompileProgram();

		static GLenum GetEnumFromShaderUnitType(eShaderUnitType type);

		void AnalyzeShaderCode(eShaderUnitType type);

		std::unordered_map<::pe::core::storage::String, UniformInfo> Uniforms;
		std::unordered_map<::pe::core::storage::String, OutputInfo> Outputs;
		GLuint ProgramHandle;
		::pe::core::utils::EnumArray<::pe::core::storage::String, eShaderUnitType> ShaderCode;

		::pe::core::storage::String VertexProgramPath;
		::pe::core::storage::String GeometryProgramPath;
		::pe::core::storage::String FragmentProgramPath;
	};
}
