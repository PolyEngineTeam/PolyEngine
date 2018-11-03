#pragma once

#include <Engine.hpp>

#include "Dialogs/IDialog.h"

class IComponentDialog : public IDialog
{
public:
	// Adds selected components to given entity and returns list of selected components' ids
	virtual Poly::Dynarray<size_t> AddComponents(Poly::Entity* predefinedEntity = nullptr, Poly::Dynarray<size_t> predefinedComponents = {}) = 0;
	// Removes selected components from entity.
	virtual void RemoveComponents(Poly::Entity* predefinedEntity = nullptr, Poly::Dynarray<size_t> predefinedComponents = {}) = 0;
};