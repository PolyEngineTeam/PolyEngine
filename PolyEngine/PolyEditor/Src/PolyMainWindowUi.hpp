#pragma once

#include <QMenu>

#include "PolyMainWindow.hpp"

class PolyMainWindowUi
{
public:
	PolyMainWindowUi(PolyMainWindow* window);
	~PolyMainWindowUi();

private:
	void InitMenuBar(PolyMainWindow* window);
	void InitToolbars(PolyMainWindow* window);
	void InitDockWidgets(PolyMainWindow* window);

	QMenuBar *MenuBar;
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