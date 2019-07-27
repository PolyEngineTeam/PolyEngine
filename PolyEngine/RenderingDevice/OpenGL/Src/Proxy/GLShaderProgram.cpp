#include <PolyRenderingDeviceGLPCH.hpp>

#include <Proxy/GLShaderProgram.hpp>
#include <Common/GLUtils.hpp>

SILENCE_MSVC_WARNING(4805, "Warning originates in std::regex");
#include <regex>
UNSILENCE_MSVC_WARNING()

using namespace Poly;

const String SHADERS_INCLUDE_DIR = "Shaders/";

GLShaderProgram::GLShaderProgram(const String& compute)
{
	gConsole.LogDebug("Creating shader program {}", compute);
	ProgramHandle = glCreateProgram();
	if (ProgramHandle == 0) {
		ASSERTE(false, "Creation of shader program failed! Exiting...");
	}

	LoadShader(eShaderUnitType::COMPUTE, compute);
	CompileShader(eShaderUnitType::COMPUTE);
	
	CompileProgram();

	for (eShaderUnitType type : IterateEnum<eShaderUnitType>())
		AnalyzeShaderCode(type);
}

GLShaderProgram::GLShaderProgram(const String& vertex, const String& fragment)
{
	FragmentProgramPath = fragment;

	gConsole.LogDebug("Creating shader program {} {}", vertex, fragment);
	ProgramHandle = glCreateProgram();
	if (ProgramHandle == 0)
	{
		ASSERTE(false, "Creation of shader program failed! Exiting...");
	}

	LoadShader(eShaderUnitType::VERTEX, vertex);
	CompileShader(eShaderUnitType::VERTEX);
	
	LoadShader(eShaderUnitType::FRAGMENT, fragment);
	CompileShader(eShaderUnitType::FRAGMENT);
	
	CompileProgram();

	for (eShaderUnitType type : IterateEnum<eShaderUnitType>())
		AnalyzeShaderCode(type);
}

GLShaderProgram::GLShaderProgram(const String& vertex, const String& geometry, const String& fragment)
{
	gConsole.LogDebug("Creating shader program {} {} {}", vertex, geometry, fragment);
	ProgramHandle = glCreateProgram();
	if (ProgramHandle == 0) {
		ASSERTE(false, "Creation of shader program failed! Exiting...");
	}

	LoadShader(eShaderUnitType::VERTEX, vertex);
	CompileShader(eShaderUnitType::VERTEX);

	LoadShader(eShaderUnitType::GEOMETRY, geometry);
	CompileShader(eShaderUnitType::GEOMETRY);

	LoadShader(eShaderUnitType::FRAGMENT, fragment);
	CompileShader(eShaderUnitType::FRAGMENT);

	CompileProgram();

	for(eShaderUnitType type : IterateEnum<eShaderUnitType>())
			AnalyzeShaderCode(type);
}

void GLShaderProgram::BindProgram() const
{
	glUseProgram(ProgramHandle);
}

void GLShaderProgram::Validate()
{
	int status = 0;
	glValidateProgram(ProgramHandle);

	glGetProgramiv(ProgramHandle, GL_VALIDATE_STATUS, &status);
	if (status == 0) {
		GLint infoLogLength = 0;
		glGetProgramiv(ProgramHandle, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<char> errorMessage;
		errorMessage.resize(static_cast<size_t>(infoLogLength + 1));
		glGetProgramInfoLog(ProgramHandle, infoLogLength, NULL, &errorMessage[0]);
		gConsole.LogError("Program validation: {}", std::string(&errorMessage[0]));
		ASSERTE(false, "Program validation failed!");
	}
}

void GLShaderProgram::LoadShader(eShaderUnitType type, const String& shaderName)
{
	String rawCode = LoadTextFileRelative(eResourceSource::ENGINE, shaderName);

	if (rawCode.IsEmpty())
		return;
	
	// @fixme optimize this, spliting lines of every opened file is not fastest way to do it
	std::vector<String> includedFiles;
	std::vector<String> linesToProcess = rawCode.Split("\n");
	StringBuilder sb;
	size_t lineCounter = 0;
	while(lineCounter < linesToProcess.size())
	{
		String currLine = linesToProcess[lineCounter].GetTrimmed();

		std::regex includeRegex(R"(^#pragma include\s*\"(\w.*)\")", std::regex::ECMAScript); // regex: ^#pragma include\s*"(\w.*)"
		auto regex_begin = std::cregex_iterator(currLine.GetCStr(), currLine.GetCStr() + currLine.GetLength(), includeRegex);
		auto regex_end = std::cregex_iterator();

		if (regex_begin != regex_end)
		{
			for (std::cregex_iterator i = regex_begin; i != regex_end; ++i)
			{
				const std::cmatch& match = *i;

				String includePath = SHADERS_INCLUDE_DIR + String(match[1].str().c_str());
				ASSERTE(std::find(includedFiles.begin(), includedFiles.end(), includePath) == includedFiles.end(), "LoadShader failed, found recursive includes!");
				includedFiles.push_back(includePath);
					
				try
				{
					String rawInclude = LoadTextFileRelative(eResourceSource::ENGINE, includePath);
					auto splitedInclude = rawInclude.Split('\n');
					linesToProcess.insert(linesToProcess.begin() + lineCounter + 1, splitedInclude.begin(), splitedInclude.end());
				}
				catch(FileIOException& ex)
				{
					gConsole.LogError("GLShaderProgram::LoadShader shaderName: {}, type: {}, failed loading include: {}", shaderName, (size_t)type, includePath);
					gConsole.LogError("GLShaderProgram::LoadShader Exception Message: {}", ex.what());
					throw ex;
				}
			}
		}
		else
		{
			sb.Append(currLine).Append("\n");
		}

		lineCounter++;
	}

	ShaderCode[type] = sb.GetString();

	String compiledPath = EvaluateFullResourcePath(eResourceSource::ENGINE, shaderName + String(".dump"));
	gConsole.LogInfo("GLShaderProgram::LoadShader debugPath: {}", compiledPath);
	SaveTextFileRelative(eResourceSource::ENGINE, shaderName + String(".dump"), ShaderCode[type]);
}

void GLShaderProgram::CompileShader(GLShaderProgram::eShaderUnitType type)
{
	GLuint shader = glCreateShader(GetEnumFromShaderUnitType(type));
	if (shader == 0)
	{
		ASSERTE(false, "Creation of shader failed!");
	}

	const char *code = ShaderCode[type].GetCStr();

	glShaderSource(shader, 1, &code, NULL);
	glCompileShader(shader);

	int compileStatus = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

	if (compileStatus == 0)
	{
		int infoLogLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<char> errorMessage;
		errorMessage.resize(static_cast<size_t>(infoLogLength + 1));
		glGetShaderInfoLog(shader, infoLogLength, NULL, &errorMessage[0]);
		gConsole.LogError("Shader compilation: {}", std::string(&errorMessage[0]));
		ASSERTE(false, "Shader compilation failed!");
	}

	glAttachShader(ProgramHandle, shader);
	glDeleteShader(shader);
	CHECK_GL_ERR();
}

void GLShaderProgram::CompileProgram()
{
	glLinkProgram(ProgramHandle);
	int linkStatus = 0;

	glGetProgramiv(ProgramHandle, GL_LINK_STATUS, &linkStatus);

	if (linkStatus == 0) {
		GLint infoLogLength = 0;
		glGetProgramiv(ProgramHandle, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<char> errorMessage;
		errorMessage.resize(static_cast<size_t>(infoLogLength + 1));
		glGetProgramInfoLog(ProgramHandle, infoLogLength, NULL, &errorMessage[0]);
		gConsole.LogError("Program linking: {}", std::string(&errorMessage[0]));
		ASSERTE(false, "Program linking failed!");
	}
}

unsigned int GLShaderProgram::GetProgramHandle() const
{
	return ProgramHandle;
}

void GLShaderProgram::RegisterUniform(const String& type, const String& name)
{
	if (Uniforms.find(name) != Uniforms.end())
		return;

	GLint location = 0;
	location = glGetUniformLocation(ProgramHandle, name.GetCStr());
	if (location == -1)
	{
		gConsole.LogError("Invalid uniform location for {}. Probably optimized out.", name);
		CHECK_GL_ERR();
		return;
	}
	Uniforms[name] = UniformInfo(type, location);
	CHECK_GL_ERR();
}

void GLShaderProgram::SetUniform(const String& name, int val)
{
	auto it = Uniforms.find(name);
	if (it != Uniforms.end())
	{
		bool isTypeValid = it->second.TypeName == "int" || it->second.TypeName == "sampler2D" || it->second.TypeName == "samplerCube";
		if (!isTypeValid)
		{
			gConsole.LogError("Invalid uniform type int for {}", name);
		}
		HEAVY_ASSERTE(isTypeValid, "Invalid uniform type!");
		glUniform1i(it->second.Location, val);
	}
	else
	{
		gConsole.LogError("Not found Uniform {} type int, in {}", name, FragmentProgramPath);
	}
}

void GLShaderProgram::SetUniform(const String& name, uint val)
{
	auto it = Uniforms.find(name);
	if (it != Uniforms.end())
	{
		bool isTypeValid = it->second.TypeName == "uint";
		if (!isTypeValid)
		{
			gConsole.LogError("Invalid uniform type uint for {}", name);
		}
		HEAVY_ASSERTE(isTypeValid, "Invalid uniform type!");
		glUniform1i(it->second.Location, val);
	}
	else
	{
		gConsole.LogError("Not found Uniform {} type uint, in {}", name, FragmentProgramPath);
	}
}

void GLShaderProgram::SetUniform(const String& name, float val)
{
	auto it = Uniforms.find(name);
	if (it != Uniforms.end())
	{
		HEAVY_ASSERTE(it->second.TypeName == "float", "Invalid uniform type!");
		glUniform1f(it->second.Location, val);
	}
	else
	{
		gConsole.LogError("Not found Uniform {} type float, in {}", name, FragmentProgramPath);
	}
}

void GLShaderProgram::SetUniform(const String & name, float val1, float val2)
{
	auto it = Uniforms.find(name);
	if (it != Uniforms.end())
	{
		HEAVY_ASSERTE(it->second.TypeName == "vec2", "Invalid uniform type!");
		glUniform2f(it->second.Location, val1, val2);
	}
	else
	{
		gConsole.LogError("Not found Uniform {} type vec2, in {}", name, FragmentProgramPath);
	}
}

void GLShaderProgram::SetUniform(const String& name, const Vector& val)
{
	auto it = Uniforms.find(name);
	if (it != Uniforms.end())
	{
		HEAVY_ASSERTE(it->second.TypeName == "vec4", "Invalid uniform type!");
		glUniform4f(it->second.Location, val.X, val.Y, val.Z, val.W);
	}
	else
	{
		gConsole.LogError("Not found Uniform {} type vec4, in {}", name, FragmentProgramPath);
	}
}

void GLShaderProgram::SetUniform(const String& name, const Color& val)
{
	auto it = Uniforms.find(name);
	if (it != Uniforms.end())
	{
		HEAVY_ASSERTE(it->second.TypeName == "vec4", "Invalid uniform type!");
		glUniform4f(it->second.Location, val.R, val.G, val.B, val.A);
	}
	else
	{
		gConsole.LogError("Not found Uniform {} type vec4, in {}", name, FragmentProgramPath);
	}
}

void GLShaderProgram::SetUniform(const String& name, const Matrix& val)
{
	auto it = Uniforms.find(name);
	if (it != Uniforms.end())
	{
		HEAVY_ASSERTE(it->second.TypeName == "mat4", "Invalid uniform type!");
		glUniformMatrix4fv(it->second.Location, 1, GL_FALSE, val.GetTransposed().GetDataPtr());
	}
	else
	{
		gConsole.LogError("Not found Uniform {} type mat4, in {}", name, FragmentProgramPath);
	}
}

void GLShaderProgram::BindSampler(const String& name, int samplerID, int textureID)
{
	HEAVY_ASSERTE(samplerID >= 0, "Invalid sampler ID!");
	HEAVY_ASSERTE(textureID > 0, "Invalid texture resource ID!");

	glActiveTexture(GL_TEXTURE0 + samplerID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	SetUniform(name, samplerID);
}

void GLShaderProgram::BindSamplerCube(const String& name, int samplerID, int cubemapID)
{
	HEAVY_ASSERTE(samplerID >= 0, "Invalid sampler ID!");
	HEAVY_ASSERTE(cubemapID > 0, "Invalid cubemap resource ID!");

	glActiveTexture(GL_TEXTURE0 + samplerID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);
	SetUniform(name, samplerID);
}

GLenum GLShaderProgram::GetEnumFromShaderUnitType(eShaderUnitType type)
{
	switch (type)
	{
		case eShaderUnitType::VERTEX: return GL_VERTEX_SHADER;
		case eShaderUnitType::GEOMETRY: return GL_GEOMETRY_SHADER;
		case eShaderUnitType::FRAGMENT: return GL_FRAGMENT_SHADER;
		case eShaderUnitType::COMPUTE: return GL_COMPUTE_SHADER;
		default:
			ASSERTE(false, "Invalid type!");
			return -1;
	}
}

void GLShaderProgram::AnalyzeShaderCode(eShaderUnitType type)
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

		size_t index = 0;
		for (std::cregex_iterator i = regex_begin; i != regex_end; ++i)
		{
			const std::cmatch& match = *i;
			ASSERTE(match.size() == 3, "Invalid regex result when parsing outs!");
			gConsole.LogDebug("Out {} of type {} found.", match[2].str(), match[1].str());

			//TODO remove unnecessary string copy
			Outputs.insert(std::make_pair(String(match[2].str().c_str()), OutputInfo(String(match[1].str().c_str()), index++)));
		}
	}
}
