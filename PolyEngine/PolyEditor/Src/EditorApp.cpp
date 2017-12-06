#include "PolyEditorPCH.hpp"

#include <QFileSystemModel>
#include <QFileSystemWatcher>
#include <QTimer>

#include <Core.hpp>

// ---------------------------------------------------------------------------------------------------------
EditorApp::EditorApp(int argc, char *argv[]) :
	QApplication(argc, argv)
{
	Engine = std::make_unique<Poly::Engine>();
	
	Ui = new PolyEditorUi();
	EventFilter = new GlobalEventFilter();

	installEventFilter(EventFilter);

	SetupUpdateTimer();
	SetupConsoleOutput();
	InitializeEngine();
	
	Poly::gConsole.LogDebug("PolyEditor succesfully initialized.");
}

// ---------------------------------------------------------------------------------------------------------
EditorApp::~EditorApp()
{
	delete Ui;
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
