#pragma once

#include <QtWidgets/QMainWindow>

#include <Engine.hpp>

#include "PolyDockWidget.hpp"

namespace Ui 
{
	class EditorMainWindowClass;
}

class PolyViewportWidget;

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

	PolyDockWidget<PolyViewportWidget>* ViewportWidget;

private slots:
	void UpdatePhase();
};
