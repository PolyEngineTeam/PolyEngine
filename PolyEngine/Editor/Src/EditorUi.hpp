#pragma once

#include <QMenu>

#include "Windows/PolyMainWindow.hpp"
#include "Widgets/ViewportWidget.hpp"
#include "Widgets/LoggerWidget.hpp"

class EditorUi : public QObject
{
	friend class DockManager;
	friend class PolyWindow;
	Q_OBJECT
public:
	EditorUi() = default;

	void InitMainWindow();

	std::unique_ptr<ViewportWidget> MainViewport;
	std::unique_ptr<LoggerWidget> MainLogger;

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
	void UpdateProjectFromEngine();
	void CloseProject();

	// build
	void Build();

	// help
	void ContactUs();
};