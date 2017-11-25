#pragma once

#include <QtWidgets/QMainWindow>

#include <Engine.hpp>

#include "PolyDockWidget.hpp"

namespace Ui 
{
	class EditorMainWindowClass;
}

class PolyViewportWidget;
class PolyConsoleWidget;

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

	// TODO: dynarray or something like this.
	PolyDockWidget<PolyViewportWidget>* ViewportWidget;
	PolyDockWidget<PolyConsoleWidget>* ConsoleWidget;

private slots:
	void UpdatePhase();
};
