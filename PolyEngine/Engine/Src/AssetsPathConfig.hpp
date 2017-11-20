#pragma once

#include <unordered_map>
#include <EnumUtils.hpp>
#include "ConfigBase.hpp"

namespace Poly
{
	class ENGINE_DLLEXPORT AssetsPathConfig : public ConfigBase
	{
		RTTI_DECLARE_TYPE_DERIVED(AssetsPathConfig, ConfigBase)
		{
			RTTI_PROPERTY(EngineAssetsPath, "EngineAssetsPath", RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY(GameAssetsPath, "GameAssetsPath", RTTI::ePropertyFlag::NONE);
		}
	public:
		AssetsPathConfig();

		const String& GetAssetsPath(eResourceSource Source) const;

	private:
		String EngineAssetsPath;
		String GameAssetsPath;
	};

	ENGINE_DLLEXPORT extern AssetsPathConfig gAssetsPathConfig;
}
