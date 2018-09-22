#pragma once

#include <Collections/String.hpp>
#include <Configs/ConfigBase.hpp>

class ProjectConfig : public Poly::ConfigBase
{
	friend class ProjectManager;

	RTTI_DECLARE_TYPE_DERIVED(ProjectConfig, Poly::RTTIBase)
	{
		RTTI_PROPERTY(Path, "Path", Poly::RTTI::ePropertyFlag::DONT_SERIALIZE);
		RTTI_PROPERTY(ProjectPath, "ProjectPath", Poly::RTTI::ePropertyFlag::DONT_SERIALIZE);
		RTTI_PROPERTY(ProjectName, "ProjectName", Poly::RTTI::ePropertyFlag::NONE);
	}

public:
	ProjectConfig(const Poly::String& path);

	// Path to directory where *.proj.json file is stored
	const Poly::String& GetProjectPath() const { return ProjectPath; }

	// project name loaded from *.proj.json
	const Poly::String& GetProjectName() const { return ProjectName; }

	// for now these functions always return Debug configuration output files
	Poly::String GetGameDllPath() const;
	Poly::String GetRenderingDeviceDllPath() const;

private:
	// Path to *.proj.json file
	Poly::String Path;

	// Path to directory where *.proj.json file is stored
	Poly::String ProjectPath;
	// project name loaded from *.proj.json
	Poly::String ProjectName;
};
