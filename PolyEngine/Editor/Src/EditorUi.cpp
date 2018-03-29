#include "PolyEditorPCH.hpp"

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <qdockwidget.h>

void EditorUi::InitMainWindow()
{
	MainWindow = new PolyMainWindow();
	Windows.PushBack(std::move(MainWindow));

	MainWindow->setObjectName(QStringLiteral("EditorMainWindowClass"));

	MainWindow->setWindowTitle(QApplication::translate("EditorMainWindowClass", "PolyEditor", Q_NULLPTR));
	MainWindow->resize(1280, 720);;
	MainWindow->setDockOptions(QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks);

	// menu bar
	MenuBar = new QMenuBar(MainWindow);
	MenuBar->setGeometry(QRect(0, 0, 1280, 21));
	MainWindow->setMenuBar(MenuBar);


		// file menu
		FileMenu = new QMenu(MenuBar);
		MenuBar->addAction(FileMenu->menuAction());
		FileMenu->setTitle(QApplication::translate("EditorMainWindowClass", "File", Q_NULLPTR));

			QuitAction = new QAction(MainWindow);
			FileMenu->addAction(QuitAction);
			QuitAction->setText(QApplication::translate("EditorMainWindowClass", "Quit", Q_NULLPTR));
			QObject::connect(QuitAction, &QAction::triggered, this, &EditorUi::Quit);


		// edit menu
		EditMenu = new QMenu(MenuBar);
		MenuBar->addAction(EditMenu->menuAction());
		EditMenu->setTitle(QApplication::translate("EditorMainWindowClass", "Edit", Q_NULLPTR));

			UndoAction = new QAction(MainWindow);
			EditMenu->addAction(UndoAction);
			UndoAction->setText(QApplication::translate("EditorMainWindowClass", "Undo", Q_NULLPTR));
			QObject::connect(UndoAction, &QAction::triggered, this, &EditorUi::Undo);

			RedoAction = new QAction(MainWindow);
			EditMenu->addAction(RedoAction);
			RedoAction->setText(QApplication::translate("EditorMainWindowClass", "Redo", Q_NULLPTR));
			QObject::connect(RedoAction, &QAction::triggered, this, &EditorUi::Redo);


		// view menu
		ViewMenu = new QMenu(MenuBar);
		MenuBar->addAction(ViewMenu->menuAction());
		ViewMenu->setTitle(QApplication::translate("EditorMainWindowClass", "View", Q_NULLPTR));

			AddWindowAction = new QAction(MainWindow);
			ViewMenu->addAction(AddWindowAction);
			AddWindowAction->setText(QApplication::translate("EditorMainWindowClass", "Add Window", Q_NULLPTR));
			QObject::connect(AddWindowAction, &QAction::triggered, this, &EditorUi::AddWindow);


		// Project menu
		ProjectMenu = new QMenu(MenuBar);
		MenuBar->addAction(ProjectMenu->menuAction());
		ProjectMenu->setTitle(QApplication::translate("EditorMainWindowClass", "Project", Q_NULLPTR));

			CreateProjectAction = new QAction(MainWindow);
			ProjectMenu->addAction(CreateProjectAction);
			CreateProjectAction->setText(QApplication::translate("EditorMainWindowClass", "Create Project", Q_NULLPTR));
			QObject::connect(CreateProjectAction, &QAction::triggered, this, &EditorUi::CreateProject);

			OpenProjectAction = new QAction(MainWindow);
			ProjectMenu->addAction(OpenProjectAction);
			OpenProjectAction->setText(QApplication::translate("EditorMainWindowClass", "Open Project", Q_NULLPTR));
			QObject::connect(OpenProjectAction, &QAction::triggered, this, &EditorUi::OpenProject);

			UpdateProjectAction = new QAction(MainWindow);
			ProjectMenu->addAction(UpdateProjectAction);
			UpdateProjectAction->setText(QApplication::translate("EditorMainWindowClass", "Update Project", Q_NULLPTR));
			QObject::connect(UpdateProjectAction, &QAction::triggered, this, &EditorUi::UpdateProject);

			UpdateProjectFromEngineAction = new QAction(MainWindow);
			ProjectMenu->addAction(UpdateProjectFromEngineAction);
			UpdateProjectFromEngineAction->setText(QApplication::translate("EditorMainWindowClass", "Update Project From Engine", Q_NULLPTR));
			QObject::connect(UpdateProjectFromEngineAction, &QAction::triggered, this, &EditorUi::UpdateProjectFromEngine);

			CloseProjectAction = new QAction(MainWindow);
			ProjectMenu->addAction(CloseProjectAction);
			CloseProjectAction->setText(QApplication::translate("EditorMainWindowClass", "Close Project", Q_NULLPTR));
			QObject::connect(CloseProjectAction, &QAction::triggered, this, &EditorUi::CloseProject);



		// build menu
		BuildMenu = new QMenu(MenuBar);
		MenuBar->addAction(BuildMenu->menuAction());
		BuildMenu->setTitle(QApplication::translate("EditorMainWindowClass", "Build", Q_NULLPTR));

			BuildAction = new QAction(MainWindow);
			BuildMenu->addAction(BuildAction);
			BuildAction->setText(QApplication::translate("EditorMainWindowClass", "Build", Q_NULLPTR));
			QObject::connect(BuildAction, &QAction::triggered, this, &EditorUi::Build);


		// help menu
		HelpMenu = new QMenu(MenuBar);
		MenuBar->addAction(HelpMenu->menuAction());
		HelpMenu->setTitle(QApplication::translate("EditorMainWindowClass", "Help", Q_NULLPTR));

			ContactUsAction = new QAction(MainWindow);
			HelpMenu->addAction(ContactUsAction);
			ContactUsAction->setText(QApplication::translate("EditorMainWindowClass", "Contact Us", Q_NULLPTR));
			QObject::connect(ContactUsAction, &QAction::triggered, this, &EditorUi::ContactUs);


	// hardcoded initialization of several widgets.
	//LoggerWidget* consoleWidget = new LoggerWidget("Assets Explorer");
	//consoleWidget->Dock(Qt::DockWidgetArea::TopDockWidgetArea, MainWindow);

	//consoleWidget = new LoggerWidget("Object Properties");
	//consoleWidget->Dock(Qt::DockWidgetArea::TopDockWidgetArea, MainWindow); //we need to create new base class for this -> someone created all as loggerwidgets

	//ViewportWidget* viewportWidget = new ViewportWidget("Viewport", MainWindow);
	//MainWindow->AddWidget(Qt::DockWidgetArea::LeftDockWidgetArea, viewportWidget, true);

	LoggerWidget* consoleWidget = new LoggerWidget("Console");
	MainWindow->AddWidget(Qt::DockWidgetArea::RightDockWidgetArea, consoleWidget, true);

	MainWindow->show();
}

void EditorUi::Quit()
{
}

void EditorUi::Undo()
{
}

void EditorUi::Redo()
{
}

void EditorUi::AddWindow()
{
	PolyWindow* window = new PolyWindow();
	//Windows.PushBack(window);
	window->show();
}

void EditorUi::CreateProject()
{
	CreateProjectDialog dialog;
	dialog.exec();

	if (dialog.OperationCanceled())
		return;

	gApp->ProjectMgr.Create(&dialog.GetProjectName().toStdString()[0],
		&dialog.GetProjectDirectory().toStdString()[0],
		&dialog.GetEngineDirectory().toStdString()[0]);
}

void EditorUi::OpenProject()
{
	Poly::gConsole.LogInfo("");
	Poly::gConsole.LogInfo("Project opening started...");

	QFileDialog fileDialog;
	fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
	fileDialog.setFileMode(QFileDialog::Directory);
	if (fileDialog.exec() == QDialog::Accepted)
		gApp->ProjectMgr.Open(&fileDialog.selectedFiles()[0].toStdString()[0]);

	Poly::gConsole.LogInfo("Project opening ended.");
}

void EditorUi::UpdateProject()
{
	Poly::gConsole.LogInfo("");
	Poly::gConsole.LogInfo("Project updating started...");

	gApp->ProjectMgr.Update();

	Poly::gConsole.LogInfo("Project updating ended.");
}

void EditorUi::UpdateProjectFromEngine()
{
	Poly::gConsole.LogInfo("");
	Poly::gConsole.LogInfo("Project updating from engine started...");

	QFileDialog fileDialog;
	fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
	fileDialog.setFileMode(QFileDialog::Directory);
	if (fileDialog.exec() == QDialog::Accepted)
		gApp->ProjectMgr.Update(&fileDialog.selectedFiles()[0].toStdString()[0]);

	Poly::gConsole.LogInfo("Project updating from engine  ended.");
}

void EditorUi::CloseProject()
{
	gApp->ProjectMgr.Close();
}

void EditorUi::Build()
{
	QString program = "cmd";
	QStringList arguments;

	//QProcess *myProcess = new QProcess(this);
	//myProcess->start(program, arguments);

	//gApp->ProjectMgr.Build();
}

void EditorUi::ContactUs()
{
}
