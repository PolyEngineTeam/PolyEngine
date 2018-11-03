#include "PolyEditorPCH.hpp"

#include <Resources/ResourceManager.hpp>

RTTI_DEFINE_TYPE(ProjectConfig)

ProjectConfig::ProjectConfig(const String& path)
	: Poly::ConfigBase(path, Poly::eResourceSource::NONE), Path(path)
{
	FileName = path;

	// get Project directory path
	for (size_t i = path.GetLength(); i > 0; --i)
		if (path[i] == '/')
		{
			ProjectPath = path.Substring(i);
			break;
		}
}

String ProjectConfig::GetGameDllPath() const
{
	StringBuilder builder;

#if defined(_WIN32)
	builder.Append(ProjectPath);
	builder.Append("/Dist/Debug/");
	builder.Append(ProjectName);
	builder.Append(".dll");
#elif defined(__linux__) || defined(__APPLE__)
	builder.Append(ProjectPath)
		.Append("/Dist/Debug/")
		.Append("lib")
		.Append(ProjectName)
		.Append(".so");
#else
#error "Unsupported platform :("
#endif

	return builder.StealString();
}

String ProjectConfig::GetRenderingDeviceDllPath() const
{
	StringBuilder builder;

#if defined(_WIN32)
	builder.Append(ProjectPath);
	builder.Append("/Dist/Debug/PolyRenderingDeviceGL.dll");
#elif defined(__linux__) || defined(__APPLE__)
	builder.Append(ProjectPath)
		.Append("/Dist/Debug/libPolyRenderingDeviceGL.so");
#else
#error "Unsupported platform :("
#endif

	return builder.StealString();
}
