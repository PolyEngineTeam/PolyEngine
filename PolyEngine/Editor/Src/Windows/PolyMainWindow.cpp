#include "PolyEditorPCH.hpp"

#include <QFileDialog>

PolyMainWindow::PolyMainWindow()
{
	setWindowTitle("PolyEditor");
	resize(1280, 720);;
	setDockOptions(QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks);

	// menu bar
	MenuBar = new QMenuBar();
	setMenuBar(MenuBar);


		// file menu
		FileMenu = new QMenu();
		MenuBar->addAction(FileMenu->menuAction());
		FileMenu->setTitle("File");

			QuitAction = new QAction();
			FileMenu->addAction(QuitAction);
			QuitAction->setText("Quit");
			QObject::connect(QuitAction, &QAction::triggered, this, &PolyMainWindow::Quit);


		// edit menu
		EditMenu = new QMenu();
		MenuBar->addAction(EditMenu->menuAction());
		EditMenu->setTitle("Edit");

			UndoAction = new QAction();
			EditMenu->addAction(UndoAction);
			UndoAction->setText("Undo");
			QObject::connect(UndoAction, &QAction::triggered, this, &PolyMainWindow::Undo);

			RedoAction = new QAction();
			EditMenu->addAction(RedoAction);
			RedoAction->setText("Redo");
			QObject::connect(RedoAction, &QAction::triggered, this, &PolyMainWindow::Redo);


		// view menu
		ViewMenu = new QMenu();
		MenuBar->addAction(ViewMenu->menuAction());
		ViewMenu->setTitle("View");

			AddWindowAction = new QAction();
			ViewMenu->addAction(AddWindowAction);
			AddWindowAction->setText("Add Window");
			QObject::connect(AddWindowAction, &QAction::triggered, this, &PolyMainWindow::AddWindow);


		// Project menu
		ProjectMenu = new QMenu();
		MenuBar->addAction(ProjectMenu->menuAction());
		ProjectMenu->setTitle("Project");

			CreateProjectAction = new QAction();
			ProjectMenu->addAction(CreateProjectAction);
			CreateProjectAction->setText("Create");
			QObject::connect(CreateProjectAction, &QAction::triggered, this, &PolyMainWindow::CreateProject);

			OpenProjectAction = new QAction();
			ProjectMenu->addAction(OpenProjectAction);
			OpenProjectAction->setText("Open");
			QObject::connect(OpenProjectAction, &QAction::triggered, this, &PolyMainWindow::OpenProject);

			UpdateProjectFromEngineAction = new QAction();
			ProjectMenu->addAction(UpdateProjectFromEngineAction);
			UpdateProjectFromEngineAction->setText("Update");
			QObject::connect(UpdateProjectFromEngineAction, &QAction::triggered, this, &PolyMainWindow::UpdateProject);

			BuildProjectAction = new QAction();
			ProjectMenu->addAction(BuildProjectAction);
			BuildProjectAction->setText("Build");
			QObject::connect(BuildProjectAction, &QAction::triggered, this, &PolyMainWindow::BuildProject);

			EditProjectAction = new QAction();
			ProjectMenu->addAction(EditProjectAction);
			EditProjectAction->setText("Edit");
			QObject::connect(EditProjectAction, &QAction::triggered, this, &PolyMainWindow::EditProject);

			PlayProjectAction = new QAction();
			ProjectMenu->addAction(PlayProjectAction);
			PlayProjectAction->setText("Play");
			QObject::connect(PlayProjectAction, &QAction::triggered, this, &PolyMainWindow::PlayProject);

			CloseProjectAction = new QAction();
			ProjectMenu->addAction(CloseProjectAction);
			CloseProjectAction->setText("Close");
			QObject::connect(CloseProjectAction, &QAction::triggered, this, &PolyMainWindow::CloseProject);


		// help menu
		HelpMenu = new QMenu();
		MenuBar->addAction(HelpMenu->menuAction());
		HelpMenu->setTitle("Help");

			ContactUsAction = new QAction();
			HelpMenu->addAction(ContactUsAction);
			ContactUsAction->setText("Contact Us");
			QObject::connect(ContactUsAction, &QAction::triggered, this, &PolyMainWindow::ContactUs);
}

void PolyMainWindow::Quit()
{
}

void PolyMainWindow::Undo()
{
}

void PolyMainWindow::Redo()
{
}

void PolyMainWindow::AddWindow()
{
	PolyWindow* window = new PolyWindow();
	//Windows.PushBack(window);
	window->show();
}

void PolyMainWindow::CreateProject()
{
	CreateProjectDialog dialog;
	dialog.exec();

	if (dialog.OperationCanceled())
		return;

	gApp->ProjectMgr.Create(&dialog.GetProjectName().toStdString()[0],
		&dialog.GetProjectDirectory().toStdString()[0],
		&dialog.GetEngineDirectory().toStdString()[0]);
}

void PolyMainWindow::OpenProject()
{
	QFileDialog fileDialog;
	fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
	if (fileDialog.exec() == QDialog::Accepted)
		gApp->ProjectMgr.Open(&fileDialog.selectedFiles()[0].toStdString()[0]);
}

void PolyMainWindow::UpdateProject()
{
	QFileDialog fileDialog;
	fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
	fileDialog.setFileMode(QFileDialog::Directory);
	if (fileDialog.exec() == QDialog::Accepted)
		gApp->ProjectMgr.Update(&fileDialog.selectedFiles()[0].toStdString()[0]);
}

void PolyMainWindow::BuildProject()
{
	gApp->ProjectMgr.Build();
}

void PolyMainWindow::EditProject()
{
	gApp->ProjectMgr.Edit();
}

void PolyMainWindow::PlayProject()
{
	gApp->ProjectMgr.Play();
}

void PolyMainWindow::CloseProject()
{
	gApp->ProjectMgr.Close();
}

void PolyMainWindow::ContactUs()
{
}