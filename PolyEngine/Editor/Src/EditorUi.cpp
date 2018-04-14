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
			CreateProjectAction->setText(QApplication::translate("EditorMainWindowClass", "Create", Q_NULLPTR));
			QObject::connect(CreateProjectAction, &QAction::triggered, this, &EditorUi::CreateProject);

			OpenProjectAction = new QAction(MainWindow);
			ProjectMenu->addAction(OpenProjectAction);
			OpenProjectAction->setText(QApplication::translate("EditorMainWindowClass", "Open", Q_NULLPTR));
			QObject::connect(OpenProjectAction, &QAction::triggered, this, &EditorUi::OpenProject);

			UpdateProjectFromEngineAction = new QAction(MainWindow);
			ProjectMenu->addAction(UpdateProjectFromEngineAction);
			UpdateProjectFromEngineAction->setText(QApplication::translate("EditorMainWindowClass", "Update", Q_NULLPTR));
			QObject::connect(UpdateProjectFromEngineAction, &QAction::triggered, this, &EditorUi::UpdateProject);

			BuildProjectAction = new QAction(MainWindow);
			ProjectMenu->addAction(BuildProjectAction);
			BuildProjectAction->setText(QApplication::translate("EditorMainWindowClass", "Build", Q_NULLPTR));
			QObject::connect(BuildProjectAction, &QAction::triggered, this, &EditorUi::BuildProject);

			EditProjectAction = new QAction(MainWindow);
			ProjectMenu->addAction(EditProjectAction);
			EditProjectAction->setText(QApplication::translate("EditorMainWindowClass", "Edit", Q_NULLPTR));
			QObject::connect(EditProjectAction, &QAction::triggered, this, &EditorUi::EditProject);

			PlayProjectAction = new QAction(MainWindow);
			ProjectMenu->addAction(PlayProjectAction);
			PlayProjectAction->setText(QApplication::translate("EditorMainWindowClass", "Play", Q_NULLPTR));
			QObject::connect(PlayProjectAction, &QAction::triggered, this, &EditorUi::PlayProject);

			CloseProjectAction = new QAction(MainWindow);
			ProjectMenu->addAction(CloseProjectAction);
			CloseProjectAction->setText(QApplication::translate("EditorMainWindowClass", "Close", Q_NULLPTR));
			QObject::connect(CloseProjectAction, &QAction::triggered, this, &EditorUi::CloseProject);


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

	MainViewport = std::make_unique<ViewportWidget>("Viewport", MainWindow);
	MainWindow->AddWidget(Qt::DockWidgetArea::LeftDockWidgetArea, MainViewport.get(), true);

	MainLogger = std::make_unique<LoggerWidget>("Console", MainWindow);
	MainWindow->AddWidget(Qt::DockWidgetArea::LeftDockWidgetArea, MainLogger.get(), true);

	CmdLogger = std::make_unique<LoggerWidget>("Cmd", MainWindow);
	MainWindow->AddWidget(Qt::DockWidgetArea::LeftDockWidgetArea, CmdLogger.get(), true);

	EntityMgr = std::make_unique<EntityManagerWidget>("Entity Manager", MainWindow);
	MainWindow->AddWidget(Qt::DockWidgetArea::RightDockWidgetArea, EntityMgr.get(), true);

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
	QFileDialog fileDialog;
	fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
	if (fileDialog.exec() == QDialog::Accepted)
		gApp->ProjectMgr.Open(&fileDialog.selectedFiles()[0].toStdString()[0]);
}

void EditorUi::UpdateProject()
{
	QFileDialog fileDialog;
	fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
	fileDialog.setFileMode(QFileDialog::Directory);
	if (fileDialog.exec() == QDialog::Accepted)
		gApp->ProjectMgr.Update(&fileDialog.selectedFiles()[0].toStdString()[0]);
}

void EditorUi::BuildProject()
{
	gApp->ProjectMgr.Build();
}

void EditorUi::EditProject()
{
	gApp->ProjectMgr.Edit();
}

void EditorUi::PlayProject()
{
	gApp->ProjectMgr.Play();
}

void EditorUi::CloseProject()
{
	gApp->ProjectMgr.Close();
}

void EditorUi::ContactUs()
{
}
