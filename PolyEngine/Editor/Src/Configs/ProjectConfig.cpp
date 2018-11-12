#include "PolyEditorPCH.hpp"

#include <Resources/ResourceManager.hpp>

RTTI_DEFINE_TYPE(ProjectConfig)

//------------------------------------------------------------------------------
ProjectConfig::ProjectConfig(const String& path, eConfiguration configuration)
	: Poly::ConfigBase(path, Poly::eResourceSource::NONE), ProjectPath(path), ProjectConfiguration(configuration)
{
	FileName = path;

	// get Project directory path
	for (size_t i = path.GetLength(); i > 0; --i)
		if (path[i] == '/')
		{
			ProjectDirPath = path.Substring(i);
			break;
		}

	Load();
}

//------------------------------------------------------------------------------
String ProjectConfig::GetGameDllPath() const
{
	StringBuilder builder;

#if defined(_WIN32)
	builder.Append(GetDistDir());
	builder.Append(ProjectName);
	builder.Append(".dll");
#elif defined(__linux__) || defined(__APPLE__)
	builder.Append(GetDistDir())
		.Append("lib")
		.Append(ProjectName)
		.Append(".so");
#else
#error "Unsupported platform :("
#endif

	return builder.StealString();
}

//------------------------------------------------------------------------------
String ProjectConfig::GetRenderingDeviceDllPath() const
{
	StringBuilder builder;

#if defined(_WIN32)
	builder.Append(GetDistDir());
	builder.Append("PolyRenderingDeviceGL.dll");
#elif defined(__linux__) || defined(__APPLE__)
	builder.Append(GetDistDir())
		.Append("libPolyRenderingDeviceGL.so");
#else
#error "Unsupported platform :("
#endif

	return builder.StealString();
}

//------------------------------------------------------------------------------
Poly::String ProjectConfig::GetDistDir() const
{
	StringBuilder builder;

	builder.Append(ProjectDirPath);
	builder.Append("/Dist/");

	switch (ProjectConfiguration)
	{
	case eConfiguration::DEBUG:
		builder.Append("Debug/");
		break;

	default:
		ASSERTE(false, "ERROR");
		break;
	}

	return builder.StealString();
}
