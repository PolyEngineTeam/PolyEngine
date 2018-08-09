#pragma once
#include "Configs/ConfigBase.hpp"

namespace Poly
{
	enum class eDebugDrawPreset
	{
		NONE = 0,
		PLAYER = BIT(1),
		DYNAMIC = BIT(2),
		STATIC = BIT(3),
		GFX = BIT(4)
	};

	/// <summary>This is class where all debug configuration variables are placed</summary>
	class ENGINE_DLLEXPORT DebugConfig : public ConfigBase
	{
		RTTI_DECLARE_TYPE_DERIVED(DebugConfig, ConfigBase)
		{
			RTTI_PROPERTY(DisplayFPS, "DisplayFPS", RTTI::ePropertyFlag::NONE);
		}
	public:
		DebugConfig() : ConfigBase("Debug Config", eResourceSource::ENGINE)
		{
			DebugDrawPresets.Clear();
			DebugDrawPresets |= eDebugDrawPreset::PLAYER;
			//DebugDrawPresets |= DebugDrawPreset::DYNAMIC;
			//DebugDrawPresets |= DebugDrawPreset::STATIC;
		}

		// Debug rendering
		EnumFlags<eDebugDrawPreset> DebugDrawPresets;
		bool DebugRender = true;
		// Rendering
		bool DisplayFPS = true;
	};

	ENGINE_DLLEXPORT extern DebugConfig gDebugConfig;
}