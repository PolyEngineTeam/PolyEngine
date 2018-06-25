#pragma once

#include "Widgets/PolyWidget.hpp"

#include "Managers/InspectorManager.hpp"

class InspectorWidgetBase : public PolyWidget
{
public:
	InspectorWidgetBase(QWidget* parent, InspectorManager* mgr)
		: PolyWidget(parent), Manager(mgr), EngineObj(mgr->EngineObj), WorldObj(mgr->WorldObj),
		SelectedEntities(mgr->SelectedEntities), Config(mgr->Config) {}

	virtual void InitializeConnections() = 0;
	virtual void Reload() = 0;
	virtual void Reset() = 0;

protected:
	InspectorManager* Manager;

	Engine*& const EngineObj;
	World*& const WorldObj;
	const Dynarray<Entity*>& const SelectedEntities;
	ProjectConfig*& const Config;
};