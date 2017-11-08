#pragma once
#include "ConfigBase.hpp"

namespace Poly
{
	/// <summary>This is class where all debug configuration variables are placed</summary>
	class ENGINE_DLLEXPORT DebugConfig : public ConfigBase
	{
		RTTI_DECLARE_TYPE_DERIVED(DebugConfig, ConfigBase)
		{
			RTTI_PROPERTY(DisplayFPS, "DisplayFPS", RTTI::ePropertyFlag::NONE);
		}
	public:
		DebugConfig() : ConfigBase("Debug Config", eConfigLocation::ENGINE) {}

		// Rendering
		bool DisplayFPS = true;
	};

	ENGINE_DLLEXPORT extern DebugConfig gDebugConfig;
}