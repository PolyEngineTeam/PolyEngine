#include "PolyEditorPCH.hpp"
#include "EditorApp.hpp"

#include "ui_EditorMainWindow.h"

#include <QFileSystemModel>
#include <QFileSystemWatcher>
#include <QTimer>

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
	//MainWindow->assetsTreeView->setModel(model);
	//MainWindow->assetsTreeView->setRootIndex(model->index(dir.absolutePath()));
	
	QFileSystemWatcher* watcher = new QFileSystemWatcher(this);
	watcher->addPath("console.log");
	QObject::connect(watcher, &QFileSystemWatcher::fileChanged, [&](const QString& path) 
	{ 
		ConsoleWidget->setText(path);
	});
	
	QObject::connect(MainWindow->actionQuit, &QAction::triggered, [](bool checked) { Poly::gConsole.LogError("test3");/*QApplication::quit();*/ });
	
	
	Poly::gConsole.LogError("test1");
	Poly::gConsole.LogError("test2");

	SetupUpdateTimer();

	ViewportWidget = new PolyDockWidget<PolyViewportWidget>(this, "Viewport 1", Qt::LeftDockWidgetArea);
	ViewportWidget->setFocusPolicy(Qt::ClickFocus);

	ConsoleWidget = new PolyDockWidget<PolyConsoleWidget>(this, "Console output", Qt::RightDockWidgetArea);
	ConsoleWidget->setEnabled(true);
	ConsoleWidget->setReadOnly(true);

	QObject::connect(MainWindow->actionQuit, &QAction::triggered, [](bool) { Poly::gConsole.LogError("test3");/*QApplication::quit();*/ });
	

	//gridLayout->addLayout(verticalLayout_2, 0, 0, 1, 1);
	//
	//consoleDockWidget->setWidget(dockWidgetContents);
	//EditorMainWindowClass->addDockWidget(static_cast<Qt::DockWidgetArea>(8), consoleDockWidget);
	//dockWidget = new QDockWidget(EditorMainWindowClass);
	//dockWidget->setObjectName(QStringLiteral("dockWidget"));
	//dockWidget->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	//dockWidgetContents_2 = new QWidget();
	//dockWidgetContents_2->setObjectName(QStringLiteral("dockWidgetContents_2"));
	//gridLayout_2 = new QGridLayout(dockWidgetContents_2);
	//gridLayout_2->setSpacing(6);
	//gridLayout_2->setContentsMargins(11, 11, 11, 11);
	//gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
	//assetsTreeView = new QTreeView(dockWidgetContents_2);
	//assetsTreeView->setObjectName(QStringLiteral("assetsTreeView"));
	//
	//gridLayout_2->addWidget(assetsTreeView, 0, 0, 1, 1);
	//
	//dockWidget->setWidget(dockWidgetContents_2);
	//EditorMainWindowClass->addDockWidget(static_cast<Qt::DockWidgetArea>(1), dockWidget);
	//dockWidget_2 = new QDockWidget(EditorMainWindowClass);
	//dockWidget_2->setObjectName(QStringLiteral("dockWidget_2"));
	//dockWidget_2->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	//dockWidgetContents_3 = new QWidget();
	//dockWidgetContents_3->setObjectName(QStringLiteral("dockWidgetContents_3"));
	//gridLayout_3 = new QGridLayout(dockWidgetContents_3);
	//gridLayout_3->setSpacing(6);
	//gridLayout_3->setContentsMargins(11, 11, 11, 11);
	//gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
	//verticalLayout = new QVBoxLayout();
	//verticalLayout->setSpacing(6);
	//verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
	//label_2 = new QLabel(dockWidgetContents_3);
	//label_2->setObjectName(QStringLiteral("label_2"));
	//
	//verticalLayout->addWidget(label_2);
	//
	//progressBar_2 = new QProgressBar(dockWidgetContents_3);
	//progressBar_2->setObjectName(QStringLiteral("progressBar_2"));
	//progressBar_2->setValue(24);
	//
	//verticalLayout->addWidget(progressBar_2);
	//
	//label = new QLabel(dockWidgetContents_3);
	//label->setObjectName(QStringLiteral("label"));
	//
	//verticalLayout->addWidget(label);
	//
	//progressBar = new QProgressBar(dockWidgetContents_3);
	//progressBar->setObjectName(QStringLiteral("progressBar"));
	//progressBar->setValue(24);
	//
	//verticalLayout->addWidget(progressBar);
	//
	//
	//gridLayout_3->addLayout(verticalLayout, 0, 0, 1, 1);
	//
	//dockWidget_2->setWidget(dockWidgetContents_3);
	//EditorMainWindowClass->addDockWidget(static_cast<Qt::DockWidgetArea>(2), dockWidget_2);

	ViewportWidget->InitializeViewport();
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
	ViewportWidget->Update();
}
