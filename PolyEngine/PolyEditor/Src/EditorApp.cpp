#include "PolyEditorPCH.hpp"
#include "EditorApp.hpp"
#include "ui_EditorMainWindow.h"
#include <QFileSystemModel>
#include <QFileSystemWatcher>

#include <Core.hpp>

// ---------------------------------------------------------------------------------------------------------
EditorApp::EditorApp(QWidget *parent) : 
	QMainWindow(parent),
	MainWindow(new Ui::EditorMainWindowClass)
{
	Engine = std::make_unique<Poly::Engine>();

	freopen("console.log", "w", stdout);
	setvbuf(stdout, NULL, _IONBF, 0);

	MainWindow->setupUi(this);

	//for (int i = 0; i<100; ++i)
	//	ui->consoleOutput->append(QString("dupa dupa\n dupa"));

	QDir dir("C:\\");
	QFileSystemModel* model = new QFileSystemModel();
	model->setRootPath(dir.absolutePath());
	MainWindow->assetsTreeView->setModel(model);
	MainWindow->assetsTreeView->setRootIndex(model->index(dir.absolutePath()));

	QFileSystemWatcher* watcher = new QFileSystemWatcher(this);
	watcher->addPath("console.log");
	QObject::connect(watcher, &QFileSystemWatcher::fileChanged, [&](const QString& path) 
	{ 
		MainWindow->consoleOutput->setText(path);
	});

	QObject::connect(MainWindow->actionQuit, &QAction::triggered, [](bool checked) { Poly::gConsole.LogError("test3");/*QApplication::quit();*/ });
	

	Poly::gConsole.LogError("test1");
	Poly::gConsole.LogError("test2");

	SetupUpdateTimer();

	MainWindow->ViewportWidget->InitializeViewport();
}

// ---------------------------------------------------------------------------------------------------------
EditorApp::~EditorApp()
{
	delete MainWindow;
	delete UpdateTimer;
}

// ---------------------------------------------------------------------------------------------------------
void EditorApp::SetupUpdateTimer()
{
	UpdateTimer = new QTimer(this);
	connect(UpdateTimer, SIGNAL(timeout()), this, SLOT(UpdatePhase()));
	UpdateTimer->start(0);
}

// ---------------------------------------------------------------------------------------------------------
void EditorApp::UpdatePhase()
{
	MainWindow->ViewportWidget->Update();
}
