#include "PolyEditorPCH.hpp"

#include <QFileSystemModel>
#include <QFileSystemWatcher>
#include <QTimer>
#include <Utils/Logger.hpp>

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
	CreateEngine();
	
	Poly::gConsole.LogInfo("PolyEditor succesfully initialized.");
}

// ---------------------------------------------------------------------------------------------------------
void EditorApp::SetupUpdateTimer()
{
	Updater = std::make_unique<QTimer>(this);
	connect(Updater.get(), SIGNAL(timeout()), this, SLOT(UpdatePhase()));
	Updater->start(0);
}

// ---------------------------------------------------------------------------------------------------------
void EditorApp::CreateEngine()
{
	Engine = std::make_unique<Poly::Engine>();
	emit EngineCreated();
}

// ---------------------------------------------------------------------------------------------------------
void EditorApp::UpdatePhase()
{
	Engine->Update();
}
