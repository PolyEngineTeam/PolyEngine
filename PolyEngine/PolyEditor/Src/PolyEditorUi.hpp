#pragma once

#include <QMenu>

#include "PolyMainWindow.hpp"

class PolyEditorUi
{
public:
	PolyEditorUi();
	~PolyEditorUi();

private:
	void InitMainWindow();
	void InitDockWidgets();

	PolyMainWindow* MainWindow;
	Poly::Dynarray<PolyWindow> Windows;

	Poly::Dynarray<QWidget> Widgets;

	QMenuBar* MenuBar;
		QMenu* FileMenu;
			QAction* CloseProjectAction;
			QAction* QuitAction;
		QMenu* EditMenu;
			QAction* UndoAction;
			QAction* RedoAction;
		QMenu* ViewMenu;
			QAction* AddWindowAction;
		QMenu* BuildMenu;
			QAction* BuildAction;
		QMenu* HelpMenu;
			QAction* ContactUsAction;
};