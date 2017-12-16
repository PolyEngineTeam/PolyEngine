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

private slots:
	void CloseProject();
	void Quit();
	void Undo();
	void Redo();
	void AddWindow();
	void Build();
	void ContactUs();
};