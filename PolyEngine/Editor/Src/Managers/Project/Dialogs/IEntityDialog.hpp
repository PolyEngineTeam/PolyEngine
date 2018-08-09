#pragma once

#include <Engine.hpp>

#include "Dialogs/IDialog.h"

class IEntityDialog : public IDialog
{
public:
	// Spawns entities from selected prefabs in each of selected parents on given scene and returns dynarray with newly created entities.
	virtual Poly::Dynarray<Poly::Entity*> SpawnEntities(Poly::Scene* scene, Poly::Dynarray<Poly::Entity*> predefinedParents = {}) = 0;
	// Destroys recursively selected entities.
	virtual void DestroyEntities(Poly::Dynarray<Poly::Entity*> predefinedEntities = {}) = 0;
	// Reparents all selected entities to one selected parent.
	virtual void ReparentEntities(Poly::Entity* predefinedParent = nullptr, Poly::Dynarray<Poly::Entity*> predefinedEntities = {}) = 0;
};