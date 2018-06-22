#pragma once

#include "Widgets/PolyWidget.hpp"

class InspectorWidgetBase : public PolyWidget
{
public:
	InspectorWidgetBase(QWidget* parent, World* world, const Dynarray<Entity*>& selectedEntities)
		: PolyWidget(parent), WorldObjjjj(world), SelectedEntities(selectedEntities) {}

	virtual void InitializeConnections() = 0;
	virtual void Reload() = 0;
	virtual void Reset() = 0;

protected:
	World* WorldObj;
	const Dynarray<Entity*>& SelectedEntities;
};