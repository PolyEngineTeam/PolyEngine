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

	builder.Append(ProjectPath);
	builder.Append("/Build/");
	builder.Append(ProjectName);
	builder.Append("/Debug/");
	builder.Append(ProjectName);
	builder.Append(".dll");

	return builder.StealString();
}

String ProjectConfig::GetRenderingDeviceDllPath() const
{
	StringBuilder builder;

	builder.Append(ProjectPath);
	builder.Append("/Build/");
	builder.Append(ProjectName);
	builder.Append("/Debug/polyrenderingdevice.dll");

	return builder.StealString();
}
