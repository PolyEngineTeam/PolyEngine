#include "PolyEditorPCH.hpp"

#include <Resources/ResourceManager.hpp>

RTTI_DEFINE_TYPE(ProjectConfig)

void ProjectConfig::Save()
{
	SerializeToFile(Path);
}

void ProjectConfig::Load()
{
	DeserializeFromFile(Path);
}
