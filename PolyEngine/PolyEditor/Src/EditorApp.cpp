#include "PolyEditorPCH.hpp"

#include <QFileSystemModel>
#include <QFileSystemWatcher>
#include <QTimer>

#include <Core.hpp>

EditorApp* gApp = nullptr;

// ---------------------------------------------------------------------------------------------------------
EditorApp::EditorApp(int argc, char *argv[]) :
	QApplication(argc, argv)
{
	gApp = this;

	Engine = std::make_unique<Poly::Engine>();

	installEventFilter(&EventFilter);

	SetupUpdateTimer();
	SetupConsoleOutput();
	InitializeEngine();
	
	Poly::gConsole.LogDebug("PolyEditor succesfully initialized.");
}

// ---------------------------------------------------------------------------------------------------------
EditorApp::~EditorApp()
{
	delete Updater;
}

// ---------------------------------------------------------------------------------------------------------
void EditorApp::SetupUpdateTimer()
{
	Updater = new QTimer(this);
	connect(Updater, SIGNAL(timeout()), this, SLOT(UpdatePhase()));
	Updater->start(0);
}

// ---------------------------------------------------------------------------------------------------------
void EditorApp::SetupConsoleOutput()
{
	freopen("console.log", "w", stdout);
	setvbuf(stdout, NULL, _IONBF, 0);
}

// ---------------------------------------------------------------------------------------------------------
void EditorApp::InitializeEngine()
{
}

// ---------------------------------------------------------------------------------------------------------
void EditorApp::UpdatePhase()
{
	Engine->Update();
}
