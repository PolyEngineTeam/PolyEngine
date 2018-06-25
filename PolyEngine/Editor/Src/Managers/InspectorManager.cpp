#include "PolyEditorPCH.hpp"

//------------------------------------------------------------------------------
InspectorManager::InspectorManager(EditorApp* app)
{
	//connect(app->ProjectMgr, &ProjectManager::ProjectOpened, this, &InspectorManager::ProjectOpenedSlot);
	//connect(app->ProjectMgr, &ProjectManager::ProjectClosed, this, &InspectorManager::ProjectClosedSlot);
	//
	//connect(app->EngineMgr, &EngineManager::Initialized, this, &InspectorManager::EngineInitializedSlot);
	//connect(app->EngineMgr, &EngineManager::Deinitialized, this, &InspectorManager::EngineDeinitializedSlot);
	//connect(app->EngineMgr, &EngineManager::StateChanged, this, &InspectorManager::StateChangedSlot);
}

//------------------------------------------------------------------------------
void InspectorManager::InitUi()
{
	// world inspector
	WorldInspector = new WorldInspectorWidget(gApp->Ui.MainWindow, this);
	PolyDockWindow* window = new PolyDockWindow("World Inspector", WorldInspector);
	gApp->Ui.MainWindow->AddDockWindow(Qt::DockWidgetArea::LeftDockWidgetArea, window, true);

	// resource inspector
	ResourceInspector = new ResourceInspectorWidget(gApp->Ui.MainWindow, this);
	window = new PolyDockWindow("Resource Inspector", ResourceInspector);
	gApp->Ui.MainWindow->AddDockWindow(Qt::DockWidgetArea::BottomDockWidgetArea, window, true);
	window->hide();

	// viewport inspector
	ViewportInspector = new ViewportInspectorWidget(gApp->Ui.MainWindow, this);
	window = new PolyDockWindow("Viewport Inspector", ViewportInspector);
	gApp->Ui.MainWindow->AddDockWindow(Qt::DockWidgetArea::RightDockWidgetArea, window, true);

	// entity inspector
	EntityInspector = new EntityInspectorWidget(gApp->Ui.MainWindow, this);
	window = new PolyDockWindow("Entity Inspector", EntityInspector);
	gApp->Ui.MainWindow->AddDockWindow(Qt::DockWidgetArea::RightDockWidgetArea, window, true);


	WorldInspector->InitializeConnections();
	ResourceInspector->InitializeConnections();
	ViewportInspector->InitializeConnections();
	EntityInspector->InitializeConnections();
}
