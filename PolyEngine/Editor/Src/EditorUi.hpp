#pragma once

#include "Windows/PolyMainWindow.hpp"
#include "Widgets/ViewportWidget.hpp"
#include "Widgets/LoggerWidget.hpp"

class WorldInspectorWidget;
class EntityInspectorWidget;
class ResourceManagerWidget;

class EditorUi : public QObject
{
	friend class DockManager;
	friend class PolyWindow;

public:
	EditorUi() = default;

	void Init();
	void AddWindow() { PolyWindow* wnd = new PolyWindow(); Windows.PushBack(wnd); wnd->show(); }

	ViewportWidget* MainViewport;
	LoggerWidget* MainLogger;
	LoggerWidget* CmdLogger;
	WorldInspectorWidget* WorldExplorer;
	EntityInspectorWidget* EntityMgr;
	ResourceManagerWidget* ResourceMgr;

private:
	PolyMainWindow* MainWindow;
	Poly::Dynarray<PolyWindow*> Windows;
	Poly::Dynarray<PolyDockWindow*> DockWindows;
};