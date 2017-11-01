#pragma once
#include "ConfigBase.hpp"

namespace Poly
{
	/// <summary>This is class where all debug configuration variables are placed</summary>
	class ENGINE_DLLEXPORT DebugConfig : public ConfigBase
	{
		RTTI_DECLARE_TYPE_DERIVED(DebugConfig, ConfigBase)
		{
			RTTI_PROPERTY(bool, DebugNormalsFlag, "DebugNormalsFlag", RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY(bool, WireframeRendering, "WireframeRendering", RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY(bool, DisplayFPS, "DisplayFPS", RTTI::ePropertyFlag::NONE);
		}
	public:
		DebugConfig() : ConfigBase("Debug", eConfigLocation::LOCAL) {}

		// Rendering
		bool DebugNormalsFlag = false;
		bool WireframeRendering = false;
		bool DisplayFPS = true;
	};

	ENGINE_DLLEXPORT extern DebugConfig gDebugConfig;
}