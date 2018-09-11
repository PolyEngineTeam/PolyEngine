#pragma once

#include <Defines.hpp>

namespace Poly
{
	class Entity;

	class ENGINE_DLLEXPORT IEditor
	{
	public:
		// Returns path to config file with assets paths (serialized AssetsPathConfig).
		virtual Poly::String GetAssetsPathConfigPath() = 0;

		// Initialize editor systems and components here.
		virtual void OnGameInit() = 0;

		// Deinitilize editor systems and components.
		virtual void OnGameDeinit() = 0;

	protected:
	};
} // namespace Poly