#pragma once

#include <QApplication>
#include <qpoint.h>

#include <Dynarray.hpp>
#include <Engine.hpp>

class PolyMainWindow;
class PolyWindow;
class PolyEditorUi;
class GlobalEventFilter;
class QDockWidget;

class EditorApp : public QApplication
{
	Q_OBJECT

public:
	EditorApp(int argc, char *argv[]);
	~EditorApp();

	bool dragging = false;
	PolyWindow* MouseOver = nullptr;
	QDockWidget* draggedWidget = nullptr;

private:
	void SetupUpdateTimer();
	void SetupConsoleOutput();
	void InitializeEngine();

	PolyEditorUi* Ui;
	GlobalEventFilter* EventFilter;

	QTimer* Updater;
	std::unique_ptr<Poly::Engine> Engine = nullptr;

private slots:
	void UpdatePhase();
};

extern EditorApp* gApp;
