#pragma once

#include <QApplication>
#include <QTimer>

#include "EditorUi.hpp"
#include "GlobalEventFilter.hpp"
#include "Managers/DockManager.hpp"
#include "Managers/ProjectManager.hpp"
#include "Managers/EngineManager.hpp"
#include "Managers/CommandManager.hpp"
#include "Managers/EditManager.hpp"
#include "Managers/InspectorManager.hpp"

#include <Engine.hpp>

class EditorApp : public QApplication
{
	Q_OBJECT

public:
	EditorApp(int argc, char *argv[]);

	EditorUi Ui;

	// TODO(squares): use unique_ptrs
	DockManager DockMgr;
	ProjectManager ProjectMgr;
	EngineManager EngineMgr;
	CommandManager CommandMgr;
	InspectorManager InspectorMgr;

	std::unique_ptr<EditManager> EditMgr;

private:
	GlobalEventFilter EventFilter;
};

extern EditorApp* gApp;
