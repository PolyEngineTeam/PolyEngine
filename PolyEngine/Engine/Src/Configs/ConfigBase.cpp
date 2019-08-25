#include <EnginePCH.hpp>

#include <Configs/ConfigBase.hpp>
#include <Resources/ResourceManager.hpp>

using namespace Poly;

RTTI_DEFINE_TYPE(Poly::ConfigBase)

ConfigBase::ConfigBase(const core::storage::String& displayName, eResourceSource location)
	: DisplayName(displayName), Location(location)
{
}

void ConfigBase::Save()
{
	SerializeToFile(EvaluateFullResourcePath(Location, GetFileName()));
}

void ConfigBase::Load()
{
	DeserializeFromFile(EvaluateFullResourcePath(Location, GetFileName()));
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
