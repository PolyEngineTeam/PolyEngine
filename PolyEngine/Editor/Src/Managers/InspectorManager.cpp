#include "PolyEditorPCH.hpp"

//------------------------------------------------------------------------------
InspectorManager::InspectorManager(EditorApp* app)
{
	connect(app->ProjectMgr, &ProjectManager::ProjectOpened, this, &InspectorManager::ProjectOpenedSlot);
	connect(app->ProjectMgr, &ProjectManager::ProjectClosed, this, &InspectorManager::ProjectClosedSlot);

	connect(app->EngineMgr, &EngineManager::Initialized, this, &InspectorManager::EngineInitializedSlot);
	connect(app->EngineMgr, &EngineManager::Deinitialized, this, &InspectorManager::EngineDeinitializedSlot);
	connect(app->EngineMgr, &EngineManager::StateChanged, this, &InspectorManager::StateChangedSlot);
}

//------------------------------------------------------------------------------
void InspectorManager::ProjectOpenedSlot(const ProjectConfig* config)
{
	emit ProjectOpened(config);
}

//------------------------------------------------------------------------------
void InspectorManager::ProjectClosedSlot()
{
	emit ProjectClosed();
}

//------------------------------------------------------------------------------
void InspectorManager::EngineInitializedSlot(World* world)
{
	emit EngineInitialized(world);
}

//------------------------------------------------------------------------------
void InspectorManager::EngineDeinitializedSlot()
{
	emit EngineDeinitialized();
}

//------------------------------------------------------------------------------
void InspectorManager::StateChangedSlot(eEngineState state)
{
	emit StateChanged(state);
}
