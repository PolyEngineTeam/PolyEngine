#pragma once

#include "PolyWindow.hpp"

class PolyMainWindow : public PolyWindow
{
public:
	PolyMainWindow(QWidget* parent = nullptr);
	~PolyMainWindow();

private:
	void SetupUi();
	void RetranslateUi(QMainWindow *EditorMainWindowClass);

	QAction *actionQuit;
	QMenuBar *menuBar;
	QMenu *menuFile;
	QMenu *menuEdit;
	QMenu *menuHelp;
};