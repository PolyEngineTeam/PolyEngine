#pragma once

#include <RTTI/RTTI.hpp>
#include <Collections/String.hpp>

using namespace Poly;

class ProjectConfig : public RTTIBase
{
	friend class ProjectManager;

	RTTI_DECLARE_TYPE_DERIVED(ProjectConfig, RTTIBase)
	{
		RTTI_PROPERTY(Path, "Path", RTTI::ePropertyFlag::DONT_SERIALIZE);
		RTTI_PROPERTY(ProjectPath, "ProjectPath", RTTI::ePropertyFlag::DONT_SERIALIZE);
		RTTI_PROPERTY(ProjectName, "ProjectName", RTTI::ePropertyFlag::NONE);
	}

public:
	ProjectConfig(const String& path);

	void Save();
	void Load();

	// for now these functions always return Debug configuration output files
	String GetGameDllPath() const;
	String GetRenderingDeviceDllPath() const;

private:
	// Path to *.proj.json file
	String Path;
	// Path to directory where *.proj.json file is stored
	String ProjectPath;

	// project name loaded from *.proj.json
	String ProjectName;
};
