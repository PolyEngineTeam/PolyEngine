#pragma once

#include <QtWidgets/QMainWindow>

#include <Engine.hpp>

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

	QTimer* UpdateTimer;
	Ui::EditorMainWindowClass* MainWindow;
	std::unique_ptr<Poly::Engine> Engine = nullptr;


private slots:
	void UpdatePhase();
};
