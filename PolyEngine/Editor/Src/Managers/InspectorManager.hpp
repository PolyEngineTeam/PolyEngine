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

	//		slots
	void ProjectOpened(const ProjectConfig* config);
	void ProjectClosed();

	void EngineInitialized(World* world);
	void EngineDeinitialized();
	void StateChanged(eEngineState state);

	void WorldChanged(World* world);
	void EntitiesSpawned(Dynarray<Entity*> entities);
	void EntitiesDestroyed();
	void EntitiesModified();
	void EntitiesReparented(Entity* parent);
	void EntitiesSelectionChanged(Dynarray<Entity*> entities);

	void ComponentsAdded(Dynarray<ComponentBase*> components);
	void ComponentsRemoved(Dynarray<ComponentBase*> components);

signals:
	//		project signals
	void ProjectOpened();
	void ProjectClosed();

	//		engine signals
	void EngineInitialized();
	void EngineDeinitialized();
	void StateChanged();

	//		world signals
	void WorldChanged();

	//		entity signals
	// Entities passed as arg are the new entities, selected entities lis is set to new entities.
	void EntitiesSpawned();
	// Currently selected entities are destroyed, selected entities list is cleared.
	void EntitiesDestroyed();
	// Currently selected entities were modified; any field within entity/component changed. 
	void EntitiesModified();
	// Currently selected entities are reparented to entity passed as arg.
	void EntitiesReparented();
	// Selected entities list is changed.
	void EntitiesSelectionChanged();
	
	//		component signals
	void ComponentsAdded();
	void ComponentsRemoved();

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
};