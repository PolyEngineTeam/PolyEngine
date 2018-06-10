#pragma once

#include <Core.hpp>

namespace Poly
{
	enum class eEngineState
	{
		EDIT,
		GAMEPLAY,
		NONE,
		_COUNT
	};

	class ENGINE_DLLEXPORT IEditor
	{
	public:
		// Returns path to config file with assets paths (serialized AssetsPathConfig).
		virtual const Poly::String& GetAssetsPathConfigPath() = 0;

		// It was not meant to use in engine.
		virtual void  SetAssetsPathConfigPath(const String& path) = 0;

		// Initialize editor systems and components here.
		virtual void Init() = 0;

		// Deinitilize editor systems and components.
		virtual void Deinit() = 0;

		// Get all ID's of currently selected entties.
		virtual const Dynarray<UniqueID>& GetSelectedEntitiesIds() = 0;
		
		// Set entities selection.
		virtual void SetSelectedEntitiesIds(Dynarray<UniqueID>) = 0;

		// If we are in edit mode and user used gizmo then this method is called.
		virtual void UpdateInspectors() = 0;

		virtual void SetEngineState(eEngineState state) { EngineState = state; }
		virtual eEngineState GetEngineState() { return EngineState; }

	protected:
		eEngineState EngineState = eEngineState::NONE;
	};
} // namespace Poly