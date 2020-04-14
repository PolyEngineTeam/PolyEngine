#include <pe/engine/EnginePCH.hpp>

#include <pe/engine/config/ConfigBase.hpp>
#include <pe/engine/res/ResourceManager.hpp>

RTTI_DEFINE_TYPE(::pe::engine::config::ConfigBase)

namespace pe::engine::config {
ConfigBase::ConfigBase(const core::storage::String& displayName, eResourceSource location)
	: DisplayName(displayName), Location(location)
{
}

void ConfigBase::Save()
{
	SerializeToFile(res::EvaluateFullResourcePath(Location, GetFileName()));
}

void ConfigBase::Load()
{
	DeserializeFromFile(res::EvaluateFullResourcePath(Location, GetFileName()));
	// For now, ensure newest state of config file.
	Save(); 
}

const core::storage::String& ConfigBase::GetFileName() const
{
	if (FileName.GetLength() == 0)
	{
		if(Location == eResourceSource::NONE)
			FileName = core::storage::String(GetTypeInfo().GetTypeName()) + core::storage::String(".json");
		else
			FileName = core::storage::String("Configs/") + core::storage::String(GetTypeInfo().GetTypeName()) + core::storage::String(".json");
	}
		
	return FileName;
}
}
