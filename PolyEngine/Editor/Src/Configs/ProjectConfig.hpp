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

	ProjectConfig(const core::storage::String& path, eConfiguration configuration);
	~ProjectConfig();

	// Path to directory where *.proj.json file is stored
	const core::storage::String& GetProjectPath() const { return ProjectDirPath; }

	// project name loaded from *.proj.json
	const core::storage::String& GetProjectName() const { return ProjectName; }

	// for now these functions always return Debug configuration output files
	core::storage::String GetGameDllPath() const;
	core::storage::String GetRenderingDeviceDllPath() const;

private:
	core::storage::String GetDistDir() const;

	// project name loaded from *.proj.json
	core::storage::String ProjectName;
	// Path to *.proj.json file
	core::storage::String ProjectPath;
	// Path to directory where *.proj.json file is stored
	core::storage::String ProjectDirPath;
	// project config configuration
	eConfiguration ProjectConfiguration;

	std::unique_ptr<Poly::AssetsPathConfig> AssetsPath;
};
