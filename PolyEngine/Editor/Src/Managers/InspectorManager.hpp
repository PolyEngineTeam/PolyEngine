#pragma once

#include <qobject.h>

#include <ECS/World.hpp>

#include "Managers/EngineManager.hpp"
#include "Configs/ProjectConfig.hpp"

using namespace Poly;

class EditorApp;

class EntityInspectorWidget;
class PrefabInspectorWidget;
class ResourceInspectorWidget;
class ViewportInspectorWidget;
class WorldComponentsInspectorWidget;
class WorldInspectorWidget;

class InspectorManager : public QObject
{
	Q_OBJECT

public:
	InspectorManager(EditorApp* app);

	void InitUi();

	std::unique_ptr<IRenderingDevice> GetRenderingDevice() { return ViewportInspector->GetRenderingDevice(); }
	IEditor* GetEditor() { return ViewportInspector; }

signals:
	void ProjectOpened(const ProjectConfig* config);
	void ProjectClosed();
	void EngineInitialized(World* world);
	void EngineDeinitialized();
	void StateChanged(eEngineState state);

private:
	WorldInspectorWidget* WorldInspector;
	WorldComponentsInspectorWidget* WorldComponentsInspector;
	ResourceInspectorWidget* ResourceInspector;
	PrefabInspectorWidget* PrefabInspector;
	ViewportInspectorWidget* ViewportInspector;
	EntityInspectorWidget* EntityInspector;

private slots:
	void ProjectOpenedSlot(const ProjectConfig* config);
	void ProjectClosedSlot();
	void EngineInitializedSlot(World* world);
	void EngineDeinitializedSlot();
	void StateChangedSlot(eEngineState state);
};