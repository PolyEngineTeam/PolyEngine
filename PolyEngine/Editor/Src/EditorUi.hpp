#pragma once

#include <QMenu>
#include <qstatusbar.h>

#include "Windows/PolyMainWindow.hpp"
#include "Widgets/ViewportWidget.hpp"
#include "Widgets/LoggerWidget.hpp"
#include "Widgets/EntityManagerWidget.hpp"
#include "Widgets/WorldExplorerWidget.hpp"

class EditorUi : public QObject
{
	friend class DockManager;
	friend class PolyWindow;
	Q_OBJECT

public:
	EditorUi() = default;

	void InitMainWindow();

	ViewportWidget* MainViewport;
	LoggerWidget* MainLogger;
	LoggerWidget* CmdLogger;
	WorldExplorerWidget* WorldExplorer;
	EntityManagerWidget* EntityMgr;

private:
	PolyMainWindow* MainWindow;
	Poly::Dynarray<PolyWindow*> Windows;
	Poly::Dynarray<PolyDockWindow*> DockWindows;

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