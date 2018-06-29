#pragma once

#include <QApplication>
#include <QtCore/qtimer.h>

#include "EditorUi.hpp"
#include "GlobalEventFilter.hpp"
#include "Managers/DockManager.hpp"
#include "Managers/ProjectManager.hpp"
#include "Managers/EngineManager.hpp"
#include "Managers/CommandManager.hpp"
#include "Managers/InspectorManager.hpp"
#include "Managers/UndoRedoManager.hpp"

#include <Engine.hpp>

class EditorApp : public QApplication
{
	Q_OBJECT

public:
	EditorApp(int argc, char *argv[]);

	EditorUi Ui;

	DockManager* DockMgr;
	CommandManager* CommandMgr;
	ProjectManager* ProjectMgr;
	EngineManager* EngineMgr;
	InspectorManager* InspectorMgr;
	UndoRedoManager* UndoRedoMgr;

private:
	GlobalEventFilter EventFilter;
};

extern EditorApp* gApp;
