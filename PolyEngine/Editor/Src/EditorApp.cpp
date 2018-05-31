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
	ProjectMgr = new ProjectManager();
	EngineMgr = new EngineManager();
	InspectorMgr = new InspectorManager(this);

	ASSERTE(!gApp, "Creating application twice?!");
	gApp = this;

	Ui.Init();
	installEventFilter(&EventFilter);

	// editMgr must be created after ui creation
	EditMgr = std::make_unique<EditManager>();
	
	Poly::gConsole.LogInfo("PolyEditor succesfully initialized.");
}
