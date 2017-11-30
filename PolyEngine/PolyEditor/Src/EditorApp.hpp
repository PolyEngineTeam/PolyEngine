#pragma once

#include <QApplication>

#include <Dynarray.hpp>
#include <Engine.hpp>

class PolyMainWindow;
class PolyWindow;

class EditorApp : public QApplication
{
	Q_OBJECT

public:
	EditorApp(int argc, char *argv[]);
	~EditorApp();

private:
	void SetupUpdateTimer();
	void SetupConsoleOutput();
	void InitializeEngine();

	PolyMainWindow* MainWindow;
	Poly::Dynarray<PolyWindow> Windows;

	QTimer* Updater;
	std::unique_ptr<Poly::Engine> Engine = nullptr;

private slots:
	void UpdatePhase();
};
