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
void InspectorManager::InitUi()
{
	// world inspector
	WorldInspector = new WorldInspectorWidget(gApp->Ui.MainWindow);
	PolyDockWindow* window = new PolyDockWindow("World Inspector", WorldInspector);
	gApp->Ui.MainWindow->AddDockWindow(Qt::DockWidgetArea::LeftDockWidgetArea, window, true);

	// world components inspector
	WorldComponentsInspector = new WorldComponentsInspectorWidget(gApp->Ui.MainWindow);
	window = new PolyDockWindow("World Components Inspector", WorldComponentsInspector);
	gApp->Ui.MainWindow->AddDockWindow(Qt::DockWidgetArea::LeftDockWidgetArea, window, true);
	window->hide();

	// resource inspector
	ResourceInspector = new ResourceInspectorWidget(gApp->Ui.MainWindow);
	window = new PolyDockWindow("Resource Inspector", ResourceInspector);
	gApp->Ui.MainWindow->AddDockWindow(Qt::DockWidgetArea::BottomDockWidgetArea, window, true);
	window->hide();

	// prefab inspector
	PrefabInspector = new PrefabInspectorWidget(gApp->Ui.MainWindow);
	window = new PolyDockWindow("Prefab Inspector", PrefabInspector);
	gApp->Ui.MainWindow->AddDockWindow(Qt::DockWidgetArea::BottomDockWidgetArea, window, true);
	window->hide();

	// viewport inspector
	ViewportInspector = new ViewportInspectorWidget(gApp->Ui.MainWindow);
	window = new PolyDockWindow("Viewport Inspector", ViewportInspector);
	gApp->Ui.MainWindow->AddDockWindow(Qt::DockWidgetArea::RightDockWidgetArea, window, true);

	// entity inspector
	EntityInspector = new EntityInspectorWidget(gApp->Ui.MainWindow);
	window = new PolyDockWindow("Entity Inspector", EntityInspector);
	gApp->Ui.MainWindow->AddDockWindow(Qt::DockWidgetArea::RightDockWidgetArea, window, true);
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
