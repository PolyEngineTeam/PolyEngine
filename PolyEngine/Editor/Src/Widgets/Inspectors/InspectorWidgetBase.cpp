#include "PolyEditorPCH.hpp"

InspectorWidgetBase::InspectorWidgetBase(QWidget* parent, InspectorManager* mgr)
	: PolyWidget(parent), 
	Manager(mgr), 
	EngineObj(mgr->EngineObj), 
	WorldObj(mgr->WorldObj),
	SelectedEntities(mgr->SelectedEntities), 
	Config(mgr->Config),
	EngineState(mgr->EngineState)
{

}
