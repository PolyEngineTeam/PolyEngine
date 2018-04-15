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
	ASSERTE(!gApp, "Creating application twice?!");
	gApp = this;

	Ui.InitMainWindow();
	installEventFilter(&EventFilter);

	// editMgr must be created after ui creation
	EditMgr = std::make_unique<EditManager>();
	
	Poly::gConsole.LogInfo("PolyEditor succesfully initialized.");
}
