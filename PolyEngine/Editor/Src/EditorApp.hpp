#pragma once

#include <QApplication>
#include <QtCore/qtimer.h>

#include "EditorUi.hpp"
#include "GlobalEventFilter.hpp"

#include <Engine.hpp>

class DockManager;
class CmdManager;
class ProjectManager;
class EngineManager;
class InspectorManager;
class UndoRedoManager;

class EditorApp : public QApplication
{
	Q_OBJECT

public:
	EditorApp(int argc, char *argv[]);

	EditorUi Ui;

	DockManager* DockMgr;
	CmdManager* CommandMgr;
	ProjectManager* ProjectMgr;
	EngineManager* EngineMgr;
	InspectorManager* InspectorMgr;
	UndoRedoManager* UndoRedoMgr;

private:
	GlobalEventFilter EventFilter;
};

extern EditorApp* gApp;
