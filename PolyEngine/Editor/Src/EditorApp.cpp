#include "PolyEditorPCH.hpp"

#include <QFileSystemModel>
#include <QFileSystemWatcher>
#include <QTimer>

#include <Core.hpp>

EditorApp* gApp = nullptr;

// ---------------------------------------------------------------------------------------------------------
EditorApp::EditorApp(int argc, char *argv[])
	: QApplication(argc, argv)
{
	DockMgr = new DockManager();
	CommandMgr = new CommandManager();
	ProjectMgr = new ProjectManager();
	EngineMgr = new EngineManager();
	InspectorMgr = new InspectorManager(this);

	ASSERTE(!gApp, "Creating application twice?!");
	gApp = this;

	Ui.Init();
	installEventFilter(&EventFilter);
	
	Poly::gConsole.LogInfo("PolyEditor succesfully initialized.");
}
