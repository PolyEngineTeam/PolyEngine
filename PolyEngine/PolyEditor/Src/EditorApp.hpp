#pragma once

#include <QApplication>

#include "EditorUi.hpp"
#include "DockManager.hpp"
#include "GlobalEventFilter.hpp"

namespace Poly { class Engine; }

class EditorApp : public QApplication
{
	Q_OBJECT

public:
	EditorApp(int argc, char *argv[]);

	DockMgr DockMgr;
	EditorUi Ui;
	
signals:
	void EngineCreated();

private:
	void SetupUpdateTimer();
	void CreateEngine();

	std::unique_ptr<QTimer> Updater = nullptr;
	std::unique_ptr<Poly::Engine> Engine = nullptr;
	GlobalEventFilter EventFilter;
	
private slots:
	void UpdatePhase();
};

extern EditorApp* gApp;
