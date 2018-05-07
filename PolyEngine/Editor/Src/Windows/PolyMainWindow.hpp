#pragma once

#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>

#include "PolyWindow.hpp"

// TODO(squares): exit app after closing this window

class PolyMainWindow : public PolyWindow
{
	Q_OBJECT

public:
	PolyMainWindow();

private:
	// main window menu actions
	QMenuBar* MenuBar;
		QMenu* FileMenu;
			QAction* QuitAction;
		QMenu* EditMenu;
			QAction* UndoAction;
			QAction* RedoAction;
		QMenu* ProjectMenu;
			QAction* CreateProjectAction;
			QAction* OpenProjectAction;
			QAction* UpdateProjectFromEngineAction;
			QAction* BuildProjectAction;
			QAction* EditProjectAction;
			QAction* PlayProjectAction;
			QAction* CloseProjectAction;
		QMenu* ToolsMenu;
			QAction* OpenVisualStudioAction;
		QMenu* ViewMenu;
			QAction* AddWindowAction;
		QMenu* HelpMenu;
			QAction* ContactUsAction;

	// toolbar

	// status bar
	QStatusBar* StatusBar;

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
	void BuildProject();
	void EditProject();
	void PlayProject();
	void CloseProject();

	// help
	void ContactUs();
};