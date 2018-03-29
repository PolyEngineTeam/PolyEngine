#pragma once

#include <QMenu>

#include "PolyMainWindow.hpp"

class EditorUi : public QObject
{
	friend class DockManager;
	friend class PolyWindow;
	Q_OBJECT
public:
	EditorUi() = default;

	void InitMainWindow();

private:
	PolyMainWindow* MainWindow;
	Poly::Dynarray<PolyWindow*> Windows;

	// main window menu actions
	QMenuBar* MenuBar;
		QMenu* FileMenu;
			QAction* QuitAction;
		QMenu* EditMenu;
			QAction* UndoAction;
			QAction* RedoAction;
		QMenu* ViewMenu;
			QAction* AddWindowAction;
		QMenu* ProjectMenu;
			QAction* CreateProjectAction;
			QAction* OpenProjectAction;
			QAction* UpdateProjectAction;
			QAction* UpdateProjectFromEngineAction;
			QAction* CloseProjectAction;
		QMenu* BuildMenu;
			QAction* BuildAction;
		QMenu* HelpMenu;
			QAction* ContactUsAction;

private slots:
	// file
	void Quit();

	// edit
	void Undo();
	void Redo();

	// view
	void AddWindow();

	// projhect
	void CreateProject();
	void OpenProject();
	void UpdateProject();
	void UpdateProjectFromEngine();
	void CloseProject();

	// build
	void Build();

	// help
	void ContactUs();
};