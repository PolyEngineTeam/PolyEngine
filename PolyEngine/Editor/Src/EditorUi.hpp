#pragma once

#include "Windows/PolyMainWindow.hpp"
#include "Widgets/ViewportWidget.hpp"
#include "Widgets/LoggerWidget.hpp"

class WorldExplorerWidget;
class EntityInspectorWidget;

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
	WorldExplorerWidget* WorldExplorer;
	EntityInspectorWidget* EntityMgr;

private:
	PolyMainWindow* MainWindow;
	Poly::Dynarray<PolyWindow*> Windows;
	Poly::Dynarray<PolyDockWindow*> DockWindows;
};