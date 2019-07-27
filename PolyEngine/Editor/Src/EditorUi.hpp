#pragma once

#include "Windows/PolyMainWindow.hpp"
#include "Widgets/LoggerWidget.hpp"

class WorldInspectorWidget;
class WorldComponentsInspectorWidget;
class EntityInspectorWidget;
class ResourceInspectorWidget;

class EditorUi : public QObject
{
	friend class DockManager;
	friend class PolyWindow;

public:
	EditorUi() = default;

	void Init();
	void AddWindow() { PolyWindow* wnd = new PolyWindow(); Windows.push_back(wnd); wnd->show(); }

	LoggerWidget* MainLogger;
	LoggerWidget* CmdLogger;

	PolyMainWindow* MainWindow;

private:
	std::vector<PolyWindow*> Windows;
	std::vector<PolyDockWindow*> DockWindows;
};