#pragma once

#include <RTTI/RTTI.hpp>
#include <Collections/String.hpp>

class ProjectConfig : public Poly::RTTIBase
{
	RTTI_DECLARE_TYPE_DERIVED(ProjectConfig, Poly::RTTIBase)
	{
		RTTI_PROPERTY(Path, "Path", Poly::RTTI::ePropertyFlag::DONT_SERIALIZE);
		RTTI_PROPERTY(ProjectName, "ProjectName", Poly::RTTI::ePropertyFlag::NONE);
	}

public:
	ProjectConfig(const Poly::String& path) : Path(path) {}

	void Save();
	void Load();

	Poly::String Path;

	Poly::String ProjectName;
};
