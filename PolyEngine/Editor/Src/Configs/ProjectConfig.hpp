#pragma once

#include <pe/core/storage/String.hpp>
#include <Configs/ConfigBase.hpp>

namespace Poly
{
	class AssetsPathConfig;
}

class ProjectConfig : public Poly::ConfigBase
{
	RTTI_DECLARE_TYPE_DERIVED(ProjectConfig, Poly::RTTIBase)
	{
		RTTI_PROPERTY(ProjectName, "ProjectName", Poly::RTTI::ePropertyFlag::NONE);
	}

public:
	enum class eConfiguration
	{
		DEBUG,
		DEBUG_FAST,
		RELEASE, 
		TESTING,
		_COUNT
	};

	ProjectConfig(const Poly::String& path, eConfiguration configuration);
	~ProjectConfig();

	// Path to directory where *.proj.json file is stored
	const Poly::String& GetProjectPath() const { return ProjectDirPath; }

	// project name loaded from *.proj.json
	const Poly::String& GetProjectName() const { return ProjectName; }

	// for now these functions always return Debug configuration output files
	Poly::String GetGameDllPath() const;
	Poly::String GetRenderingDeviceDllPath() const;

private:
	Poly::String GetDistDir() const;

	// project name loaded from *.proj.json
	Poly::String ProjectName;
	// Path to *.proj.json file
	Poly::String ProjectPath;
	// Path to directory where *.proj.json file is stored
	Poly::String ProjectDirPath;
	// project config configuration
	eConfiguration ProjectConfiguration;

	std::unique_ptr<Poly::AssetsPathConfig> AssetsPath;
};
