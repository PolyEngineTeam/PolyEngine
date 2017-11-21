#pragma once

#include <QtWidgets/QMainWindow>

namespace Ui 
{
	class EditorMainWindowClass;
}

class EditorApp : public QMainWindow
{
	Q_OBJECT

public:
	EditorApp(QWidget *parent = nullptr);
	~EditorApp();

private:
	void SetupUpdateTimer();

	Ui::EditorMainWindowClass* MainWindow;
	QTimer* UpdateTimer;


private slots:
	void UpdatePhase();
};
