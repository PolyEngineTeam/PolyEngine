#include "GLShaderProgram.hpp"
#include "GLUtils.hpp"

#include <regex>

#include <ResourceManager.hpp>

using namespace Poly;

//------------------------------------------------------------------------------
GLShaderProgram::GLShaderProgram(const String & vertex, const String & fragment)
{
	gConsole.LogDebug("Creating shader program {} {}", vertex, fragment);
	ProgramHandle = glCreateProgram();
	if (ProgramHandle == 0) {
		ASSERTE(false, "Creation of shader program failed! Exiting...");
	}
	LoadShader(eShaderUnitType::VERTEX, vertex);
	LoadShader(eShaderUnitType::FRAGMENT, fragment);
	CompileProgram();

	for (eShaderUnitType type : IterateEnum<eShaderUnitType>())
		AnalyzeShaderCode(type);
}

//------------------------------------------------------------------------------
GLShaderProgram::GLShaderProgram(const String& vertex, const String& geometry, const String& fragment)
{
	gConsole.LogDebug("Creating shader program {} {} {}", vertex, geometry, fragment);
	ProgramHandle = glCreateProgram();
	if (ProgramHandle == 0) {
		ASSERTE(false, "Creation of shader program failed! Exiting...");
	}
	LoadShader(eShaderUnitType::VERTEX, vertex);
	LoadShader(eShaderUnitType::GEOMETRY, geometry);
	LoadShader(eShaderUnitType::FRAGMENT, fragment);
	CompileProgram();

	for(eShaderUnitType type : IterateEnum<eShaderUnitType>())
			AnalyzeShaderCode(type);
}

//------------------------------------------------------------------------------
void GLShaderProgram::BindProgram() const
{
	glUseProgram(ProgramHandle);
}

//------------------------------------------------------------------------------
void GLShaderProgram::CompileProgram()
{
	glLinkProgram(ProgramHandle);
	int linkStatus = 0;

	glGetProgramiv(ProgramHandle, GL_LINK_STATUS, &linkStatus);

	if (linkStatus == 0) {
		GLint infoLogLength = 0;
		glGetProgramiv(ProgramHandle, GL_INFO_LOG_LENGTH, &infoLogLength);
		Dynarray<char> errorMessage;
		errorMessage.Resize(static_cast<size_t>(infoLogLength + 1));
		glGetProgramInfoLog(ProgramHandle, infoLogLength, NULL, &errorMessage[0]);
		gConsole.LogError("Program linking: {}", std::string(&errorMessage[0]));
		ASSERTE(false, "Program linking failed!");
	}
}

//------------------------------------------------------------------------------
void GLShaderProgram::Validate()
{
	int status = 0;
	glValidateProgram(ProgramHandle);

	glGetProgramiv(ProgramHandle, GL_VALIDATE_STATUS, &status);
	if (status == 0) {
		GLint infoLogLength = 0;
		glGetProgramiv(ProgramHandle, GL_INFO_LOG_LENGTH, &infoLogLength);
		Dynarray<char> errorMessage;
		errorMessage.Resize(static_cast<size_t>(infoLogLength + 1));
		glGetProgramInfoLog(ProgramHandle, infoLogLength, NULL, &errorMessage[0]);
		gConsole.LogError("Program validation: {}", std::string(&errorMessage[0]));
		ASSERTE(false, "Program validation failed!");
	}
}

//------------------------------------------------------------------------------
void GLShaderProgram::LoadShader(eShaderUnitType type, const String& shaderName)
{
	GLuint shader = glCreateShader(GetEnumFromShaderUnitType(type));
	if (shader == 0) {
		ASSERTE(false, "Creation of shader failed!");
	}
	
	ShaderCode[type] = LoadTextFile(GetResourcesAbsolutePath() + shaderName);

	const char *code = ShaderCode[type].GetCStr();
	glShaderSource(shader, 1, &code, NULL);
	glCompileShader(shader);

	int compileStatus = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

	if (compileStatus == 0) {
		int infoLogLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		Dynarray<char> errorMessage;
		errorMessage.Resize(static_cast<size_t>(infoLogLength + 1));
		glGetShaderInfoLog(shader, infoLogLength, NULL, &errorMessage[0]);
		gConsole.LogError("Shader compilation: {}", std::string(&errorMessage[0]));
		ASSERTE(false, "Shader compilation failed!");
	}

	glAttachShader(ProgramHandle, shader);
	glDeleteShader(shader);  // is it ok to do it here?
	CHECK_GL_ERR();
}

//------------------------------------------------------------------------------
size_t GLShaderProgram::GetProgramHandle() const
{
	return ProgramHandle;
}

//------------------------------------------------------------------------------
void GLShaderProgram::RegisterUniform(const String& type, const String& name)
{
	if (Uniforms.find(name) != Uniforms.end())
		return;

	GLint location = 0;
	location = glGetUniformLocation(ProgramHandle, name.GetCStr());
	if (location == -1)
	{
		gConsole.LogError("Invalid uniform location for {}. Probably optimized out.", name);
		return;
	}
	Uniforms[name] = UniformInfo(type, location);
	CHECK_GL_ERR();
}

//------------------------------------------------------------------------------
void GLShaderProgram::SetUniform(const String& name, int val)
{ 
	HEAVY_ASSERTE(Uniforms[name].TypeName == "int", "Invalid uniform type!");
	glUniform1i(Uniforms[name].Location, val);
}
//------------------------------------------------------------------------------
void GLShaderProgram::SetUniform(const String& name, float val)
{
	HEAVY_ASSERTE(Uniforms[name].TypeName == "float", "Invalid uniform type!");
	glUniform1f(Uniforms[name].Location, val);
}

//------------------------------------------------------------------------------
void GLShaderProgram::SetUniform(const String & name, float val1, float val2)
{
	HEAVY_ASSERTE(Uniforms[name].TypeName == "vec2", "Invalid uniform type!");
	glUniform2f(Uniforms[name].Location, val1, val2);
}

//------------------------------------------------------------------------------
void GLShaderProgram::SetUniform(const String& name, const Vector& val)
{
	HEAVY_ASSERTE(Uniforms[name].TypeName == "vec4", "Invalid uniform type!");
	glUniform4f(Uniforms[name].Location, val.X, val.Y, val.Z, val.W);
}

//------------------------------------------------------------------------------
void GLShaderProgram::SetUniform(const String& name, const Color& val)
{
	HEAVY_ASSERTE(Uniforms[name].TypeName == "vec4", "Invalid uniform type!");
	glUniform4f(Uniforms[name].Location, val.R, val.G, val.B, val.A);
}

//------------------------------------------------------------------------------
void GLShaderProgram::SetUniform(const String& name, const Matrix& val)
{
	HEAVY_ASSERTE(Uniforms[name].TypeName == "mat4", "Invalid uniform type!");
	glUniformMatrix4fv(Uniforms[name].Location, 1, GL_FALSE, val.GetTransposed().GetDataPtr());
}

//------------------------------------------------------------------------------
GLenum GLShaderProgram::GetEnumFromShaderUnitType(eShaderUnitType type)
{
	switch (type)
	{
		case eShaderUnitType::VERTEX: return GL_VERTEX_SHADER;
		case eShaderUnitType::GEOMETRY: return GL_GEOMETRY_SHADER;
		case eShaderUnitType::FRAGMENT: return GL_FRAGMENT_SHADER;
		default:
			ASSERTE(false, "Invalid type!");
			return -1;
	}
}

//------------------------------------------------------------------------------
void Poly::GLShaderProgram::AnalyzeShaderCode(eShaderUnitType type)
{
	static const std::regex uniformRegex(R"(uniform\s+(\w+)\s+(\w+)\s*;)", std::regex::ECMAScript);
	static const std::regex outRegex(R"(out\s+(\w+)\s+(\w+)\s*;)", std::regex::ECMAScript);
	//TODO parse attributes for sanity checks

	if (ShaderCode[type].GetLength() == 0)
		return;

	// Parse uniforms
	{
		auto regex_begin = std::cregex_iterator(ShaderCode[type].GetCStr(), ShaderCode[type].GetCStr() + ShaderCode[type].GetLength(), uniformRegex);
		auto regex_end = std::cregex_iterator();

		for (std::cregex_iterator i = regex_begin; i != regex_end; ++i)
		{
			const std::cmatch& match = *i;
			ASSERTE(match.size() == 3, "Invalid regex result when parsing uniforms!");
			gConsole.LogDebug("Uniform {} of type {} found.", match[2].str(), match[1].str());

			//TODO remove unnecessary string copy
			RegisterUniform(String(match[1].str().c_str()), String(match[2].str().c_str()));
		}
	}

	// Parse outputs
	if (type == eShaderUnitType::FRAGMENT)
	{
		auto regex_begin = std::cregex_iterator(ShaderCode[type].GetCStr(), ShaderCode[type].GetCStr() + ShaderCode[type].GetLength(), outRegex);
		auto regex_end = std::cregex_iterator();

		for (std::cregex_iterator i = regex_begin; i != regex_end; ++i)
		{
			const std::cmatch& match = *i;
			ASSERTE(match.size() == 3, "Invalid regex result when parsing outs!");
			gConsole.LogDebug("Out {} of type {} found.", match[2].str(), match[1].str());

			//TODO remove unnecessary string copy
			Outputs.PushBack(OutputInfo(String(match[1].str().c_str()), String(match[2].str().c_str())));
		}
	}
}
