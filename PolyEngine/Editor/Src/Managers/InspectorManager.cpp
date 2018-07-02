#include "PolyEditorPCH.hpp"

#include "Managers/CommandsImpl.hpp"

//		general
//------------------------------------------------------------------------------
InspectorManager::InspectorManager(EditorApp* app)
	: ConfigRef(Config)
{
	connect(app->ProjectMgr, &ProjectManager::ProjectOpened, this, &InspectorManager::ProjectOpenedSlot);
	connect(app->ProjectMgr, &ProjectManager::ProjectClosed, this, &InspectorManager::ProjectClosedSlot);
	
	connect(app->EngineMgr, &EngineManager::Created, this, &InspectorManager::EngineCreatedSlot);
	connect(app->EngineMgr, &EngineManager::Initialized, this, &InspectorManager::EngineInitializedSlot);
	connect(app->EngineMgr, &EngineManager::Deinitialized, this, &InspectorManager::EngineDeinitializedSlot);
	connect(app->EngineMgr, &EngineManager::StateChanged, this, &InspectorManager::StateChangedSlot);
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



//		project slots
//------------------------------------------------------------------------------
void InspectorManager::ProjectOpenedSlot(const ProjectConfig* config)
{
	Config = config;

	emit ProjectOpened();
}

//------------------------------------------------------------------------------
void InspectorManager::ProjectClosedSlot()
{
	Config = nullptr;

	emit ProjectClosed();
}



//		engine slots
//------------------------------------------------------------------------------
void InspectorManager::EngineCreatedSlot(Engine* engine)
{
	EngineObj = engine;

	emit EngineCreated();
	WorldChangedSlot(EngineObj->GetWorld());
}

//------------------------------------------------------------------------------
void InspectorManager::EngineInitializedSlot()
{
	emit EngineInitialized();
	WorldChangedSlot(EngineObj->GetWorld());
}

//------------------------------------------------------------------------------
void InspectorManager::EngineDeinitializedSlot()
{
	EngineObj = nullptr;

	emit EngineDeinitialized();
}

//------------------------------------------------------------------------------
void InspectorManager::StateChangedSlot(eEngineState state)
{
	EngineState = state;

	emit StateChanged();
}



//		world slots
//------------------------------------------------------------------------------
void InspectorManager::WorldChangedSlot(World* world)
{
	WorldObj = world;

	emit WorldChanged();
}



//		entities slots
//------------------------------------------------------------------------------
void InspectorManager::EntitiesSpawnedSlot()
{
	emit EntitiesSpawned();
}

//------------------------------------------------------------------------------
void InspectorManager::EntitiesDestroyedSlot()
{
	emit EntitiesDestroyed();
}

//------------------------------------------------------------------------------
void InspectorManager::EntitiesModifiedSlot(Command* cmd)
{
	gApp->UndoRedoMgr->AddCommand(cmd);

	emit EntitiesModified();
}

//------------------------------------------------------------------------------
void InspectorManager::EntitiesReparentedSlot()
{
	emit EntitiesReparented();
}

//------------------------------------------------------------------------------
void InspectorManager::EntitiesSelectionChangedSlot(Dynarray<Entity*> entities)
{
	// controls must be updated before signal is emitted and those controls are reset.
	QTimer::singleShot(50, this, [e = entities, object = this, old = SelectedEntities]()
		{ 
			EntitiesSelectionChangedCommand* c = new EntitiesSelectionChangedCommand();
			c->OldEntities = old;
			c->NewEntities = e;
			c->Manager = object;
			gApp->UndoRedoMgr->AddCommand(c);

			object->SelectedEntities = e;
			object->EntitiesSelectionChanged();
		});
}



//		components slots
//------------------------------------------------------------------------------
void InspectorManager::ComponentsAddedSlot(Dynarray<ComponentBase*> components)
{
	emit ComponentsAdded();
}

//------------------------------------------------------------------------------
void InspectorManager::ComponentsRemovedSlot(Dynarray<ComponentBase*> components)
{
	emit ComponentsRemoved();
}
