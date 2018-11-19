#include "PolyEditorPCH.hpp"

#include <Resources/ResourceManager.hpp>

RTTI_DEFINE_TYPE(ProjectConfig)

//------------------------------------------------------------------------------
ProjectConfig::ProjectConfig(const String& path, eConfiguration configuration)
	: Poly::ConfigBase(path, Poly::eResourceSource::NONE), ProjectPath(path), ProjectConfiguration(configuration)
	, AssetsPath(std::make_unique<Poly::AssetsPathConfig>())
{
	FileName = path;

	// get Project directory path
	for (size_t i = path.GetLength(); i > 0; --i)
		if (path[i] == '/' || path[i] == '\\')
		{
			ProjectDirPath = path.Substring(i);
			break;
		}

	Load();

	StringBuilder builder;
	builder.Append(GetDistDir());
	builder.Append("AssetsPathConfig.json");
	AssetsPath->DeserializeFromFile(builder.StealString());
}

//------------------------------------------------------------------------------
String ProjectConfig::GetGameDllPath() const
{
	return AssetsPath->GetGameLibPath();
}

//------------------------------------------------------------------------------
String ProjectConfig::GetRenderingDeviceDllPath() const
{
	return AssetsPath->GetRenderingDeviceLibPath();
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

	case eConfiguration::DEBUG_FAST:
		builder.Append("DebugFast/");
		break;

	case eConfiguration::RELEASE:
		builder.Append("Release/");
		break;

	case eConfiguration::TESTING:
		builder.Append("Testing/");
		break;

	default:
		ASSERTE(false, "ERROR");
		break;
	}

	return builder.StealString();
}
