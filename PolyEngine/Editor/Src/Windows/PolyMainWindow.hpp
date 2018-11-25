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
	std::unique_ptr<QMenu> ProjectMenu;
	std::unique_ptr<QAction> EditProjectAction;
	std::unique_ptr<QAction> PlayProjectAction;
	std::unique_ptr<QMenu> ViewMenu;
	std::unique_ptr<QAction> AddWindowAction;

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
	void EditProject();
	void PlayProject();

	// help
	void ContactUs();

protected:
	void closeEvent(QCloseEvent* event) override;
};