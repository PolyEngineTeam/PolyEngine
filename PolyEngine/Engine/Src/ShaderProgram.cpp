#include "EnginePCH.hpp"

#include "ShaderProgram.hpp"

#include <gl/glew.h>

using namespace Poly;

ShaderProgram::ShaderProgram(const String & vertex, const String & fragment)
{
	gConsole.LogDebug("Creating shader program {} {}", vertex, fragment);
	m_program = glCreateProgram();
	if (m_program == 0) {
		gConsole.LogError("Creation of shader program failed! Exiting...");
		std::exit(-1);
	}
	LoadShader(GL_VERTEX_SHADER, vertex);
	LoadShader(GL_FRAGMENT_SHADER, fragment);
	CompileProgram();
}

void ShaderProgram::BindProgram() const
{
	glUseProgram(m_program);
}

void ShaderProgram::CompileProgram()
{
	glLinkProgram(m_program);
	int linkStatus = 0;

	glGetProgramiv(m_program, GL_LINK_STATUS, &linkStatus);

	if (linkStatus == 0) {
		int infoLogLength = 0;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &infoLogLength);
		Dynarray<char> errorMessage;
		errorMessage.Resize(infoLogLength + 1);
		glGetProgramInfoLog(m_program, infoLogLength, NULL, &errorMessage[0]);
		gConsole.LogError("Program linking: {}", std::string(&errorMessage[0]));
		std::exit(-1);
	}
}

void ShaderProgram::Validate()
{
	int status = 0;
	glValidateProgram(m_program);

	glGetProgramiv(m_program, GL_VALIDATE_STATUS, &status);
	if (status == 0) {
		int infoLogLength = 0;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &infoLogLength);
		Dynarray<char> errorMessage;
		errorMessage.Resize(infoLogLength + 1);
		glGetProgramInfoLog(m_program, infoLogLength, NULL, &errorMessage[0]);
		gConsole.LogError("Program validation: {}", std::string(&errorMessage[0]));
		std::exit(-1);
	}
}

void ShaderProgram::LoadShader(int type, const String& shaderName)
{
	int shader = glCreateShader(type);
	if (shader == 0) {
		gConsole.LogError("Creation of shader failed! Exiting...");
		std::exit(-1);
	}

	String shaderCode = LoadTextFile(shaderName);

	const char *code = shaderCode.GetCStr();
	glShaderSource(shader, 1, &code, NULL);
	glCompileShader(shader);

	int compileStatus = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

	if (compileStatus == 0) {
		int infoLogLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		Dynarray<char> errorMessage;
		errorMessage.Resize(infoLogLength + 1);
		glGetShaderInfoLog(shader, infoLogLength, NULL, &errorMessage[0]);
		gConsole.LogError("Shader compilation: {}", std::string(&errorMessage[0]));
		std::exit(-1);
	}

	glAttachShader(m_program, shader);
	glDeleteShader(shader);  // is it ok to do it here?
	//CHECK_GL_ERR();
}

size_t ShaderProgram::GetProgramHandle() const
{
	return m_program;
}

void ShaderProgram::RegisterUniform(const String& name)
{
	GLint tmp = 0;
	tmp = glGetUniformLocation(m_program, name.GetCStr());
	if (tmp == -1) {
		gConsole.LogError("Invalid uniform location for {}", name);
		return;
	}
	m_uniforms[name] = tmp;
	//CHECK_GL_ERR();
}

void ShaderProgram::SetUniform(const String& name, int val)
{
	glUniform1i(m_uniforms[name], val);
}

void ShaderProgram::SetUniform(const String& name, float val)
{
	glUniform1f(m_uniforms[name], val);
}

void ShaderProgram::SetUniform(const String& name, const Vector& val)
{
	glUniform4f(m_uniforms[name], val.X, val.Y, val.Z, val.W);
}

void ShaderProgram::SetUniform(const String& name, const Matrix& val)
{
	glUniformMatrix4fv(m_uniforms[name], 1, GL_FALSE, val.GetTransposed().Data);
}
