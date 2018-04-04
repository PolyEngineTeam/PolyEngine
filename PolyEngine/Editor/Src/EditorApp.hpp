#pragma once

#include <QApplication>
#include <QTimer>

#include "EditorUi.hpp"
#include "GlobalEventFilter.hpp"
#include "Managers/DockManager.hpp"
#include "Managers/ProjectManager.hpp"
#include "Managers/EngineManager.hpp"

#include <Engine.hpp>

class EditorApp : public QApplication
{
	Q_OBJECT

public:
	EditorApp(int argc, char *argv[]);

	EditorUi Ui;

	DockManager DockMgr;
	ProjectManager ProjectMgr;
	EngineManager GameMgr;

private:
	GlobalEventFilter EventFilter;
};

extern EditorApp* gApp;
