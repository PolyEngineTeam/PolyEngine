#pragma once

#include <pe/Defines.hpp>
#include <pe/core/rtti/RTTI.hpp>
#include <pe/core/storage/String.hpp>

namespace Poly
{
	/// <summary>Enum used to identify source location of an asset.
	enum class eResourceSource
	{
		ENGINE, // engine resources location
		GAME, // game resources location
		USR_LOCAL, // user local space resources location
		NONE, // default (often runtime directory) resources location, this option will not modify path to resource
		_COUNT
	};

	class ENGINE_DLLEXPORT ConfigBase : public RTTIBase
	{
		RTTI_DECLARE_TYPE_DERIVED(ConfigBase, RTTIBase) { NO_RTTI_PROPERTY(); }
	public:
		ConfigBase(const core::storage::String& displayName, eResourceSource location);

		void Save();
		void Load();
		
		const core::storage::String& GetFileName() const;
	protected:
		mutable core::storage::String FileName;
		core::storage::String DisplayName;
		eResourceSource Location;
	};
}