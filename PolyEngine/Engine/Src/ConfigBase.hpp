#pragma once

#include "RTTI.hpp"
#include "String.hpp"

namespace Poly
{
	enum class eConfigLocation
	{
		ENGINE,
		GAME,
		LOCAL
	};

	class ENGINE_DLLEXPORT ConfigBase : public RTTIBase
	{
		RTTI_DECLARE_TYPE_DERIVED(ConfigBase, RTTIBase) { NO_RTTI_PROPERTY(); }
	public:
		ConfigBase(const String& displayName, eConfigLocation location);

		void Save();
		void Load();

	protected:
		String FileName;
		String DisplayName;
		eConfigLocation Location;
	};
}