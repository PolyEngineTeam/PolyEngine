#pragma once

#include <ConfigBase.hpp>
#include <unordered_map>
#include <EnumUtils.hpp>

class ProjectConfig : public Poly::ConfigBase
{
	RTTI_DECLARE_TYPE_DERIVED(ProjectConfig, Poly::ConfigBase)
	{
		RTTI_PROPERTY(ProjectName, "ProjectName", Poly::RTTI::ePropertyFlag::NONE);
	}

public:
	ProjectConfig(Poly::String path) : Poly::ConfigBase(path, Poly::eResourceSource::NONE){}

	Poly::String ProjectName;
};