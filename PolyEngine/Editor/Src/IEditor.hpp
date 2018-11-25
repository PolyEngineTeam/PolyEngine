#pragma once

#include <Collections/Dynarray.hpp>
#include <Editor/IEditor.hpp>

namespace PolyEditor
{
	enum class eEngineState
	{
		NONE,
		EDIT,
		GAMEPLAY,
		PAUSED_GAMEPLAY,
		_COUNT
	};

	class IEditor : public Poly::IEditor
	{
	public:
		// Get all ID's of currently selected entties.
		virtual Poly::Dynarray<Poly::Entity*> GetSelectedEntities() = 0;

		// Set entities selection.
		virtual void SetSelectedEntities(Poly::Dynarray<Poly::Entity*>) = 0;

		// If we are in edit mode and user used gizmo then this method is called.
		virtual void UpdateInspectors() = 0;

		virtual void SetEngineState(eEngineState state) = 0;
		virtual eEngineState GetEngineState() = 0;
	};
}