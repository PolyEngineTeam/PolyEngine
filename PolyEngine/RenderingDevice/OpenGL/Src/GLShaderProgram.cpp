#include "GLShaderProgram.hpp"
#include "GLUtils.hpp"

#include <regex>

#include <ResourceManager.hpp>

using namespace Poly;

//------------------------------------------------------------------------------
GLShaderProgram::GLShaderProgram(const String & vertex, const String & fragment)
{
	gConsole.LogDebug("Creating shader program {} {}", vertex, fragment);
	m_program = glCreateProgram();
	if (m_program == 0) {
		ASSERTE(false, "Creation of shader program failed! Exiting...");
	}
	LoadShader(eShaderUnitType::VERTEX, vertex);
	LoadShader(eShaderUnitType::FRAGMENT, fragment);
	CompileProgram();

	for (eShaderUnitType type : IterateEnum<eShaderUnitType>())
		if (ShaderCode[type].GetLength() > 0)
			AnalyzeShaderCode(ShaderCode[type]);
}

//------------------------------------------------------------------------------
GLShaderProgram::GLShaderProgram(const String& vertex, const String& geometry, const String& fragment)
{
	gConsole.LogDebug("Creating shader program {} {} {}", vertex, geometry, fragment);
	m_program = glCreateProgram();
	if (m_program == 0) {
		ASSERTE(false, "Creation of shader program failed! Exiting...");
	}
	LoadShader(eShaderUnitType::VERTEX, vertex);
	LoadShader(eShaderUnitType::GEOMETRY, geometry);
	LoadShader(eShaderUnitType::FRAGMENT, fragment);
	CompileProgram();

	for(eShaderUnitType type : IterateEnum<eShaderUnitType>())
		if(ShaderCode[type].GetLength() > 0)
			AnalyzeShaderCode(ShaderCode[type]);
}

//------------------------------------------------------------------------------
void GLShaderProgram::BindProgram() const
{
	glUseProgram(m_program);
}

//------------------------------------------------------------------------------
void GLShaderProgram::CompileProgram()
{
	glLinkProgram(m_program);
	int linkStatus = 0;

	glGetProgramiv(m_program, GL_LINK_STATUS, &linkStatus);

	if (linkStatus == 0) {
		GLint infoLogLength = 0;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &infoLogLength);
		Dynarray<char> errorMessage;
		errorMessage.Resize(static_cast<size_t>(infoLogLength + 1));
		glGetProgramInfoLog(m_program, infoLogLength, NULL, &errorMessage[0]);
		gConsole.LogError("Program linking: {}", std::string(&errorMessage[0]));
		ASSERTE(false, "Program linking failed!");
	}
}

//------------------------------------------------------------------------------
void GLShaderProgram::Validate()
{
	int status = 0;
	glValidateProgram(m_program);

	glGetProgramiv(m_program, GL_VALIDATE_STATUS, &status);
	if (status == 0) {
		GLint infoLogLength = 0;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &infoLogLength);
		Dynarray<char> errorMessage;
		errorMessage.Resize(static_cast<size_t>(infoLogLength + 1));
		glGetProgramInfoLog(m_program, infoLogLength, NULL, &errorMessage[0]);
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

	glAttachShader(m_program, shader);
	glDeleteShader(shader);  // is it ok to do it here?
	CHECK_GL_ERR();
}

//------------------------------------------------------------------------------
size_t GLShaderProgram::GetProgramHandle() const
{
	return m_program;
}

//------------------------------------------------------------------------------
void GLShaderProgram::RegisterUniform(const String& type, const String& name)
{
	if (m_uniforms.find(name) != m_uniforms.end())
		return;

	GLint location = 0;
	location = glGetUniformLocation(m_program, name.GetCStr());
	if (location == -1)
	{
		gConsole.LogError("Invalid uniform location for {}. Probably optimized out.", name);
		return;
	}
	m_uniforms[name] = UniformInfo(type, location);
	CHECK_GL_ERR();
}

//------------------------------------------------------------------------------
void GLShaderProgram::SetUniform(const String& name, int val)
{ 
	HEAVY_ASSERTE(m_uniforms[name].TypeName == "int", "Invalid uniform type!");
	glUniform1i(m_uniforms[name].Location, val);
}
//------------------------------------------------------------------------------
void GLShaderProgram::SetUniform(const String& name, float val)
{
	HEAVY_ASSERTE(m_uniforms[name].TypeName == "float", "Invalid uniform type!");
	glUniform1f(m_uniforms[name].Location, val);
}

//------------------------------------------------------------------------------
void GLShaderProgram::SetUniform(const String & name, float val1, float val2)
{
	HEAVY_ASSERTE(m_uniforms[name].TypeName == "vec2", "Invalid uniform type!");
	glUniform2f(m_uniforms[name].Location, val1, val2);
}

//------------------------------------------------------------------------------
void GLShaderProgram::SetUniform(const String& name, const Vector& val)
{
	HEAVY_ASSERTE(m_uniforms[name].TypeName == "vec4", "Invalid uniform type!");
	glUniform4f(m_uniforms[name].Location, val.X, val.Y, val.Z, val.W);
}

//------------------------------------------------------------------------------
void GLShaderProgram::SetUniform(const String& name, const Color& val)
{
	HEAVY_ASSERTE(m_uniforms[name].TypeName == "vec4", "Invalid uniform type!");
	glUniform4f(m_uniforms[name].Location, val.R, val.G, val.B, val.A);
}

//------------------------------------------------------------------------------
void GLShaderProgram::SetUniform(const String& name, const Matrix& val)
{
	HEAVY_ASSERTE(m_uniforms[name].TypeName == "mat4", "Invalid uniform type!");
	glUniformMatrix4fv(m_uniforms[name].Location, 1, GL_FALSE, val.GetTransposed().GetDataPtr());
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
void Poly::GLShaderProgram::AnalyzeShaderCode(const String& code)
{
	std::regex uniformRegex(R"(uniform\s+(\w+)\s+(\w+)\s*;)", std::regex::ECMAScript);

	auto regex_begin = std::cregex_iterator(code.GetCStr(), code.GetCStr() + code.GetLength(), uniformRegex);
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
