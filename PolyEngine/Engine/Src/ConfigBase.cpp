#include "EnginePCH.hpp"
#include "ConfigBase.hpp"

RTTI_DEFINE_TYPE(Poly::ConfigBase)

Poly::ConfigBase::ConfigBase(const String& displayName, eConfigLocation location)
	: DisplayName(displayName), Location(location)
{
	FileName = GetTypeInfo().GetTypeName();
}

void Poly::ConfigBase::Save()
{
}

void Poly::ConfigBase::Load()
{
}
