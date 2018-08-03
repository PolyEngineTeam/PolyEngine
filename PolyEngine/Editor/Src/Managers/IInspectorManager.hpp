#pragma once

#include <QtCore/qobject.h>

#include "Widgets/Inspectors/EntityInspectorWidget.hpp"
#include "Widgets/Inspectors/WorldInspectorWidget.hpp"
#include "Widgets/Inspectors/ResourceInspectorWidget.hpp"
#include "Widgets/Inspectors/ViewportInspectorWidget.hpp"

class IInspectorManager : public QObject
{
	Q_OBJECT

public:
	IInspectorManager() = default;

	void SetWorldInspector(std::unique_ptr<WorldInspectorWidget> worldInspector) { WorldInspector = std::move(worldInspector); }
	void SetResourceInspector(std::unique_ptr<ResourceInspectorWidget> resourceInspector) { ResourceInspector = std::move(resourceInspector); }
	void SetViewportInspector(std::unique_ptr<ViewportInspectorWidget> viewportInspector) { ViewportInspector = std::move(viewportInspector); }
	void SetEntityInspector(std::unique_ptr<EntityInspectorWidget> entityInspector) { EntityInspector = std::move(entityInspector); }


slots
	virtual void ProjectOpened(const ProjectConfig& config) = 0;
	virtual void ProjectClosed() = 0;

	virtual void EngineInitialized(Engine* engine) = 0;
	virtual void EngineDeinitialized() = 0;
	virtual void EngineStateChanged(eEngineState state) = 0;

	virtual void SceneChanged(Scene* scene) = 0;

	virtual void EntitiesSpawned() = 0;
	virtual void EntitiesDestroyed() = 0;
	virtual void EntitiesModified(std::unique_ptr<Command> cmd) = 0;
	virtual void EntitiesReparented() = 0;
	virtual void EntitiesSelectionChanged(Dynarray<Entity*> entities) = 0;

	virtual void ComponentsAdded(Dynarray<ComponentBase*> components) = 0;
	virtual void ComponentsRemoved(Dynarray<ComponentBase*> components) = 0;

signals:
	//		project signals
	void ProjectOpenedSignal();
	void ProjectClosedSignal();

	//		engine signals
	void EngineInitializedSignalSignal();
	void EngineDeinitializedSignalSignal();
	void StateChangedSignal();

	//		world signals
	void ScenehangedSignal();

	//		entity signals
	void EntitiesSpawnedSignal();
	void EntitiesDestroyedSignal();
	void EntitiesModifiedSignal();
	void EntitiesReparentedSignal();
	void EntitiesSelectionChangedSignal();

	//		component signals
	void ComponentsAddedSignal();
	void ComponentsRemovedSignal();

	//		other
	void UpdateSignal();
	void ReloadSignal();

protected:
	std::unique_ptr<WorldInspectorWidget> WorldInspector;
	std::unique_ptr<ResourceInspectorWidget> ResourceInspector;
	std::unique_ptr<ViewportInspectorWidget> ViewportInspector;
	std::unique_ptr<EntityInspectorWidget> EntityInspector;
};