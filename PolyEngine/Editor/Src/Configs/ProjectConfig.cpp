#include "PolyEditorPCH.hpp"

#include <Resources/ResourceManager.hpp>

RTTI_DEFINE_TYPE(ProjectConfig)

ProjectConfig::ProjectConfig(const String& path)
	: Path(path)
{
	// get Project directory path
	for (size_t i = path.GetLength(); i > 0; --i)
		if (path[i] == '/')
		{
			ProjectPath = path.Substring(i);
			break;
		}
}

void ProjectConfig::Save()
{
	SerializeToFile(Path);
}

void ProjectConfig::Load()
{
	DeserializeFromFile(Path);
}

String ProjectConfig::GetGameDllPath() const
{
	StringBuilder builder;

#ifdef WIN32
	builder.Append(ProjectPath);
	builder.Append("/Build/");
	builder.Append(ProjectName);
	builder.Append("/Debug/");
	builder.Append(ProjectName);
	builder.Append(".dll");
#else
	builder.Append(ProjectPath)
			.Append("/Build/")
			.Append(ProjectName)
			.Append("/lib")
			.Append(ProjectName)
			.Append(".so");
#endif

	return builder.StealString();
}

String ProjectConfig::GetRenderingDeviceDllPath() const
{
	StringBuilder builder;

#ifdef WIN32
	builder.Append(ProjectPath);
	builder.Append("/Build/");
	builder.Append(ProjectName);
	builder.Append("/Debug/polyrenderingdevice.dll");
#else
	builder.Append(ProjectPath)
			.Append("/Build/")
			.Append(ProjectName)
			.Append("/libpolyrenderingdevice.so");
#endif

	return builder.StealString();
}
