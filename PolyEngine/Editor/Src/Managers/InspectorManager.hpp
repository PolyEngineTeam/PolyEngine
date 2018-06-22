#pragma once

#include <qobject.h>

#include <ECS/World.hpp>

#include "Managers/EngineManager.hpp"
#include "Configs/ProjectConfig.hpp"
#include "Widgets/Inspectors/ViewportInspectorWidget.hpp"

using namespace Poly;

class EditorApp;

class InspectorManager : public QObject
{
	Q_OBJECT

public:
	InspectorManager(EditorApp* app);

	void InitUi();

	std::unique_ptr<IRenderingDevice> GetRenderingDevice() { return ViewportInspector->GetRenderingDevice(); }
	IEditor* GetEditor() { return ViewportInspector; }

signals:
	//		project signals
	void ProjectOpened(const ProjectConfig* config);
	void ProjectClosed();

	//		engine signals
	void EngineInitialized(World* world);
	void EngineDeinitialized();
	void StateChanged(eEngineState state);

	//		world signals
	void WorldChanged(World* world);

	//		entity signals
	// Entities passed as arg are the new entities, selected entities lis is set to new entities.
	void EntitiesSpawned(Dynarray<Entity*> entities);
	// Currently selected entities are destroyed, selected entities list is cleared.
	void EntitiesDestroyed();
	// Currently selected entities were modified; any field within entity/component changed. 
	void EntitiesModified();
	// Currently selected entities are reparented to entity passed as arg.
	void EntitiesReparented(Entity* parent);
	// Selected entities list is changed.
	void EntitiesSelectionChanged(Dynarray<Entity*> entities);
	
	//		component signals
	void ComponentsAdded(Dynarray<ComponentBase*> components);
	void ComponentsRemoved(Dynarray<ComponentBase*> components);

	//		other
	void Update();
	void Reload();
	void Reset();

private:
	WorldInspectorWidget* WorldInspector;
	WorldComponentsInspectorWidget* WorldComponentsInspector;
	ResourceInspectorWidget* ResourceInspector;
	ViewportInspectorWidget* ViewportInspector;
	EntityInspectorWidget* EntityInspector;

private slots:
	void ProjectOpenedSlot(const ProjectConfig* config);
	void ProjectClosedSlot();
	void EngineInitializedSlot(World* world);
	void EngineDeinitializedSlot();
	void StateChangedSlot(eEngineState state);
};