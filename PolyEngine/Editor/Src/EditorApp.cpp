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
	
	
	Poly::gConsole.LogInfo("PolyEditor succesfully initialized.");
}
