#pragma once

#include <QApplication>
#include <qpoint.h>

#include <Dynarray.hpp>
#include <Engine.hpp>

#include "PolyEditorUi.hpp"
#include "PolyDockManager.hpp"
#include "GlobalEventFilter.hpp"

class PolyMainWindow;
class PolyWindow;
class PolyEditorUi;
class GlobalEventFilter;
class QDockWidget;
class PolyWidget;

class EditorApp : public QApplication
{
	Q_OBJECT

public:
	EditorApp(int argc, char *argv[]);
	~EditorApp();

	PolyDockManager DockManager;
	PolyEditorUi Ui;
	GlobalEventFilter EventFilter;

private:
	void SetupUpdateTimer();
	void SetupConsoleOutput();
	void InitializeEngine();


	QTimer* Updater;
	std::unique_ptr<Poly::Engine> Engine = nullptr;

private slots:
	void UpdatePhase();
};

extern EditorApp* gApp;
