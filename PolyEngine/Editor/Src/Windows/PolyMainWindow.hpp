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
	std::unique_ptr<QMenuBar> MenuBar;
		std::unique_ptr<QMenu> FileMenu;
			std::unique_ptr<QAction> QuitAction;
		std::unique_ptr<QMenu> EditMenu;
			std::unique_ptr<QAction> UndoAction;
			std::unique_ptr<QAction> RedoAction;
		std::unique_ptr<QMenu> ProjectMenu;
			std::unique_ptr<QAction> CreateProjectAction;
			std::unique_ptr<QAction> OpenProjectAction;
			std::unique_ptr<QAction> UpdateProjectFromEngineAction;
			std::unique_ptr<QAction> BuildProjectAction;
			std::unique_ptr<QAction> EditProjectAction;
			std::unique_ptr<QAction> PlayProjectAction;
			std::unique_ptr<QAction> CloseProjectAction;
		std::unique_ptr<QMenu> ToolsMenu;
			std::unique_ptr<QAction> OpenVisualStudioAction;
		std::unique_ptr<QMenu> ViewMenu;
			std::unique_ptr<QAction> AddWindowAction;
		std::unique_ptr<QMenu> HelpMenu;
			std::unique_ptr<QAction> ContactUsAction;

	// toolbar

	// status bar
	std::unique_ptr<QStatusBar> StatusBar;

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