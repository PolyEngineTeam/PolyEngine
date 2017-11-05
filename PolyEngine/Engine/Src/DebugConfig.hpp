#pragma once
#include "ConfigBase.hpp"

namespace Poly
{
	struct ENGINE_DLLEXPORT TestStruct : public RTTIBase
	{
		RTTI_DECLARE_TYPE_DERIVED(TestStruct, RTTIBase)
		{
			RTTI_PROPERTY(bool, a, "a", RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY(bool, b, "b", RTTI::ePropertyFlag::NONE);
		}

		bool a = true;
		bool b = false;
	};

	/// <summary>This is class where all debug configuration variables are placed</summary>
	class ENGINE_DLLEXPORT DebugConfig : public ConfigBase
	{
		RTTI_DECLARE_TYPE_DERIVED(DebugConfig, ConfigBase)
		{
			RTTI_PROPERTY(bool, DisplayFPS, "DisplayFPS", RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY(TestStruct, test, "test", RTTI::ePropertyFlag::NONE);
		}
	public:
		DebugConfig() : ConfigBase("Debug", eConfigLocation::LOCAL) {}

		// Rendering
		bool DisplayFPS = true;
		TestStruct test;
	};

	ENGINE_DLLEXPORT extern DebugConfig gDebugConfig;
}