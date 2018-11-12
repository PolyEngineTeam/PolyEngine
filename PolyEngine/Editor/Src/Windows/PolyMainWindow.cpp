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

			EditProjectAction = std::make_unique<QAction>();
			ProjectMenu->addAction(EditProjectAction.get());
			EditProjectAction->setText("Edit");
			connect(EditProjectAction.get(), &QAction::triggered, this, &PolyMainWindow::EditProject);

			PlayProjectAction = std::make_unique<QAction>();
			ProjectMenu->addAction(PlayProjectAction.get());
			PlayProjectAction->setText("Play");
			connect(PlayProjectAction.get(), &QAction::triggered, this, &PolyMainWindow::PlayProject);
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

void PolyMainWindow::EditProject()
{
	gApp->ProjectMgr->Edit();
}

void PolyMainWindow::PlayProject()
{
	gApp->ProjectMgr->Play();
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
			event->accept();
		}
		else if (resBtn == QMessageBox::No)
		{
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
