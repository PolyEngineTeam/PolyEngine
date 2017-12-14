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
	Engine = std::make_unique<Poly::Engine>();
	emit EngineInitialized();
}

// ---------------------------------------------------------------------------------------------------------
void EditorApp::UpdatePhase()
{
	Engine->Update();
}
