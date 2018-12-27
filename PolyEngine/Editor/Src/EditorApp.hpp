#pragma once

#include <QApplication>
#include <QtCore/qtimer.h>

#include "EditorUi.hpp"
#include "GlobalEventFilter.hpp"

#include <Engine.hpp>

class DockManager;
class CmdManager;
class ProjectManager;
class InspectorManager;
class CommandManager;

namespace editor::controllers
{
	class IEngineController;
}

class EditorApp : public QApplication
{
	Q_OBJECT

public:
	EditorApp(int argc, char *argv[]);
	~EditorApp();

	EditorUi Ui;

	DockManager* DockMgr;
	CmdManager* CmdMgr;
	ProjectManager* ProjectMgr;
	InspectorManager* InspectorMgr;
	CommandManager* CommandMgr;

	std::unique_ptr<editor::controllers::IEngineController> EngineController;

private:
	GlobalEventFilter EventFilter;
};

extern EditorApp* gApp;
