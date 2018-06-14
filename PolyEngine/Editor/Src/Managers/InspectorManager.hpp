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

	friend class ViewportInspectorWidget;
	friend class EntityInspectorWidget;
	friend class ResourceInspectorWidget;
	friend class WorldComponentsInspectorWidget;
	friend class WorldInspectorWidget;

public:
	InspectorManager(EditorApp* app);

	void InitUi();

	std::unique_ptr<IRenderingDevice> GetRenderingDevice() { return ViewportInspector->GetRenderingDevice(); }
	IEditor* GetEditor() { return ViewportInspector; }

signals:
	// project signals
	void ProjectOpened(const ProjectConfig* config);
	void ProjectClosed();

	// engine signals
	void EngineInitialized(World* world);
	void EngineDeinitialized();
	void StateChanged(eEngineState state);

	// entity signals
	void EntitiesSelectionChanged(Dynarray<Entity*> entities);
	void EntitiesReparented(Dynarray<Entity*> entities);

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