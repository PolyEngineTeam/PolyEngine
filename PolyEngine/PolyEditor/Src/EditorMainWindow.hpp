#pragma once

#include <QtWidgets/QMainWindow>

namespace Ui 
{
	class EditorMainWindowClass;
}

class EditorApplication : public QMainWindow
{
	Q_OBJECT

public:
	EditorApplication(QWidget *parent = nullptr);
	~EditorApplication();

private:
	void SetupUpdateTimer();

	Ui::EditorMainWindowClass* MainWindow;
	QTimer* UpdateTimer;


private slots:
	void UpdatePhase();
};
