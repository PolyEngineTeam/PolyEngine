#include "PolyEditorPCH.hpp"

#include <Resources/ResourceManager.hpp>

RTTI_DEFINE_TYPE(ProjectConfig)

void ProjectConfig::Save()
{
	SerializeToFile(Poly::EvaluateFullResourcePath(Poly::eResourceSource::NONE, Path));
}

void ProjectConfig::Load()
{
	DeserializeFromFile(Poly::EvaluateFullResourcePath(Poly::eResourceSource::NONE, Path));
}
