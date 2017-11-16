#pragma once

#include "RTTI.hpp"
#include "String.hpp"

namespace Poly
{
	/// <summary>Enum used to identify source of asset.
	enum class eResourceSource
	{
		ENGINE,
		GAME,
		NONE,
		USR_LOCAL,
		_COUNT
	};

	class ENGINE_DLLEXPORT ConfigBase : public RTTIBase
	{
		RTTI_DECLARE_TYPE_DERIVED(ConfigBase, RTTIBase) { NO_RTTI_PROPERTY(); }
	public:
		ConfigBase(const String& displayName, eResourceSource location);

		void Save();
		void Load();
		
		const String& GetFileName() const;
	protected:
		mutable String FileName;
		String DisplayName;
		eResourceSource Location;
	};
}