#pragma once

#include <QtCore/qobject.h>

#include "Configs/ProjectConfig.hpp"

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

	virtual const ProjectConfig& GetProjectConfig() = 0;
	virtual const Poly::Engine* GetEngine() = 0;
	virtual const Poly::Scene* GetScene() = 0;
	virtual const Poly::Dynarray<const Poly::Entity*> GetSelectedEntities() = 0;

	// Opens SceneDialog.
	virtual void AddScene() = 0;
	// Opens SceneDialog.
	virtual void RemoveScene() = 0;
	// Change current scene to given.
	virtual void ChangeScene(Poly::Scene* scene) = 0;

	// Opens EntityDialog.
	virtual void SpawnEntities() = 0;
	// Opens EntityDialog.
	virtual void DestroyEntities() = 0;
	// Opens EntityDialog.
	virtual void ReparentEntities() = 0;
	// Changes currently selected entities to given.
	virtual void ChangeEntitiesSelection(Poly::Dynarray<Poly::Entity*> entities) = 0;
	// Calls 'do" or something like that and stores cmd at undo/redo stack.
	virtual void ModifyEntity(std::unique_ptr<Command> cmd) = 0;

	// Opens ComponentDialog.
	virtual void AddComponents() = 0;
	// Opens ComponentDialog
	virtual void RemoveComponents() = 0;

slots
	virtual void ProjectOpened(const ProjectConfig& config) = 0;
	virtual void ProjectClosed() = 0;

	virtual void EngineInitialized(Engine* engine) = 0;
	virtual void EngineDeinitialized() = 0;
	virtual void EngineStateChanged(eEngineState state) = 0;

signals:
	//		project signals
	void ProjectOpenedSignal();
	void ProjectClosedSignal();

	//		engine signals
	void EngineInitializedSignalSignal();
	void EngineDeinitializedSignalSignal();
	void StateChangedSignal();

	//		scene signals
	void SceneAddedSignal();
	void SceneRemovedSignal();
	void SceneChangedSignal();

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