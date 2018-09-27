#include "PolyEditorPCH.hpp"

#include <QFileDialog>
#include <QtWidgets/QStyleFactory>
#include <QtWidgets/QMessageBox>

PolyMainWindow::PolyMainWindow()
	: PolyWindow()
{
	setWindowTitle("PolyEditor");
	resize(1280, 720);;

	// menu bar
	MenuBar = std::make_unique<QMenuBar>();
	setMenuBar(MenuBar.get());


		// file menu
		FileMenu = std::make_unique<QMenu>();
		MenuBar->addAction(FileMenu->menuAction());
		FileMenu->setTitle("File");

			QuitAction = std::make_unique<QAction>();
			FileMenu->addAction(QuitAction.get());
			QuitAction->setText("Quit");
			connect(QuitAction.get(), &QAction::triggered, this, &PolyMainWindow::Quit);


		// view menu
		ViewMenu = std::make_unique<QMenu>();
		MenuBar->addAction(ViewMenu->menuAction());
		ViewMenu->setTitle("View");

			AddWindowAction = std::make_unique<QAction>();
			ViewMenu->addAction(AddWindowAction.get());
			AddWindowAction->setText("Add Window");
			connect(AddWindowAction.get(), &QAction::triggered, this, &PolyMainWindow::AddWindow);


		// Project menu
		ProjectMenu = std::make_unique<QMenu>();
		MenuBar->addAction(ProjectMenu->menuAction());
		ProjectMenu->setTitle("Project");

			CreateProjectAction = std::make_unique<QAction>();
			ProjectMenu->addAction(CreateProjectAction.get());
			CreateProjectAction->setText("Create");
			connect(CreateProjectAction.get(), &QAction::triggered, this, &PolyMainWindow::CreateProject);

			OpenProjectAction = std::make_unique<QAction>();
			ProjectMenu->addAction(OpenProjectAction.get());
			OpenProjectAction->setText("Open");
			connect(OpenProjectAction.get(), &QAction::triggered, this, &PolyMainWindow::OpenProject);

			UpdateProjectFromEngineAction = std::make_unique<QAction>();
			ProjectMenu->addAction(UpdateProjectFromEngineAction.get());
			UpdateProjectFromEngineAction->setText("Update");
			connect(UpdateProjectFromEngineAction.get(), &QAction::triggered, this, &PolyMainWindow::UpdateProject);

			EditProjectAction = std::make_unique<QAction>();
			ProjectMenu->addAction(EditProjectAction.get());
			EditProjectAction->setText("Edit");
			connect(EditProjectAction.get(), &QAction::triggered, this, &PolyMainWindow::EditProject);

			PlayProjectAction = std::make_unique<QAction>();
			ProjectMenu->addAction(PlayProjectAction.get());
			PlayProjectAction->setText("Play");
			connect(PlayProjectAction.get(), &QAction::triggered, this, &PolyMainWindow::PlayProject);

			CloseProjectAction = std::make_unique<QAction>();
			ProjectMenu->addAction(CloseProjectAction.get());
			CloseProjectAction->setText("Close");
			connect(CloseProjectAction.get(), &QAction::triggered, this, &PolyMainWindow::CloseProject);

	EditProjectAction->setVisible(false);
	PlayProjectAction->setVisible(false);
	CloseProjectAction->setVisible(false);
}

void PolyMainWindow::Quit()
{
	close();
}

void PolyMainWindow::Undo()
{
}

void PolyMainWindow::Redo()
{
}

void PolyMainWindow::AddWindow()
{
	gApp->Ui.AddWindow();
}

void PolyMainWindow::CreateProject()
{
	CreateProjectDialog dialog(this);
	dialog.exec();

	if (dialog.OperationCanceled())
		return;

	gApp->ProjectMgr->Create(&dialog.GetProjectName().toStdString()[0],
		&dialog.GetProjectDirectory().toStdString()[0],
		&dialog.GetEngineDirectory().toStdString()[0]);
}

void PolyMainWindow::OpenProject()
{
	QFileDialog fileDialog(this);
	fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
	if (fileDialog.exec() == QDialog::Accepted)
	{
		gApp->ProjectMgr->Open(&fileDialog.selectedFiles()[0].toStdString()[0]);

		CreateProjectAction->setVisible(false);
		OpenProjectAction->setVisible(false);
		UpdateProjectFromEngineAction->setVisible(false);

		EditProjectAction->setVisible(true);
		PlayProjectAction->setVisible(true);
		CloseProjectAction->setVisible(true);
	}
}

void PolyMainWindow::UpdateProject()
{
	QFileDialog fileDialog(this);
	fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
	fileDialog.setFileMode(QFileDialog::Directory);
	if (fileDialog.exec() == QDialog::Accepted)
		gApp->ProjectMgr->Update(&fileDialog.selectedFiles()[0].toStdString()[0]);
}

void PolyMainWindow::BuildProject()
{
	gApp->ProjectMgr->Build();
}

void PolyMainWindow::EditProject()
{
	gApp->ProjectMgr->Edit();
}

void PolyMainWindow::PlayProject()
{
	gApp->ProjectMgr->Play();
}

void PolyMainWindow::CloseProject()
{
	gApp->ProjectMgr->Close();

	CreateProjectAction->setVisible(true);
	OpenProjectAction->setVisible(true);
	UpdateProjectFromEngineAction->setVisible(true);

	EditProjectAction->setVisible(false);
	PlayProjectAction->setVisible(false);
	CloseProjectAction->setVisible(false);
}

void PolyMainWindow::ContactUs()
{
}

void PolyMainWindow::closeEvent(QCloseEvent* event)
{
	if (gApp->ProjectMgr->IsOpened())
	{
		QMessageBox::StandardButton resBtn = QMessageBox::question(this, "PolyEditor",
			tr("Do you want to save your changes before closing?\n"),
			QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
			QMessageBox::Yes);

		if (resBtn == QMessageBox::Yes)
		{
			gApp->ProjectMgr->Save();
			gApp->ProjectMgr->Close();
			event->accept();
		}
		else if (resBtn == QMessageBox::No)
		{
			gApp->ProjectMgr->Close();
			event->accept();
		}
		else
			event->ignore();

	}
	else
	{
		QMessageBox::StandardButton resBtn = QMessageBox::question(this, "PolyEditor",
			tr("Do you really want to exit?\n"),
			QMessageBox::No | QMessageBox::Yes,
			QMessageBox::Yes);

		if (resBtn != QMessageBox::Yes)
			event->ignore();
		else 
			event->accept();
	}
}
