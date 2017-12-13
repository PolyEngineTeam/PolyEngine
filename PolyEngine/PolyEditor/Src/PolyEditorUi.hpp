#pragma once

#include <QMenu>

#include "PolyMainWindow.hpp"

class PolyEditorUi : public QObject
{
	friend class PolyDockManager;
	Q_OBJECT
public:
	PolyEditorUi();
	~PolyEditorUi();

	void InitMainWindow();

private:
	void InitDockWidgets();

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