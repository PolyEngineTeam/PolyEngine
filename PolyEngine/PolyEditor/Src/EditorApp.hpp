#pragma once

#include <QApplication>

#include <Engine.hpp>

#include "PolyEditorUi.hpp"
#include "PolyDockManager.hpp"
#include "GlobalEventFilter.hpp"

class EditorApp : public QApplication
{
	Q_OBJECT

public:
	EditorApp(int argc, char *argv[]);
	~EditorApp();

	PolyDockManager DockManager;
	PolyEditorUi Ui;
	GlobalEventFilter EventFilter;

signals:
	void EngineCreated();

private:
	void SetupUpdateTimer();
	void SetupConsoleOutput();
	void CreateEngine();

	QTimer* Updater;
	std::unique_ptr<Poly::Engine> Engine = nullptr;

private slots:
	void UpdatePhase();
};

extern EditorApp* gApp;
