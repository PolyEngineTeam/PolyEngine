#pragma once

#include <QtWidgets/QMainWindow>

namespace Ui {
	class EditorMainWindowClass;
}

class EditorMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	EditorMainWindow(QWidget *parent = nullptr);

private:
	Ui::EditorMainWindowClass* ui;
};
