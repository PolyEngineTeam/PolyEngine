#include "PolyEditorPCH.hpp"

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <qdockwidget.h>

void EditorUi::Init()
{
	MainWindow = new PolyMainWindow();
	Windows.PushBack(std::move(MainWindow));

	// main logger
	MainLogger = new LoggerWidget(eLoggerType::CONSOLE);
	PolyDockWindow* window = new PolyDockWindow("Console", MainLogger);
	MainWindow->AddDockWindow(Qt::DockWidgetArea::RightDockWidgetArea, window, true);
	window->hide();

	// command logger
	CmdLogger = new LoggerWidget(eLoggerType::CONSOLE);
	window = new PolyDockWindow("Cmd", CmdLogger);
	MainWindow->AddDockWindow(Qt::DockWidgetArea::RightDockWidgetArea, window, true);
	window->hide();

	gApp->InspectorMgr->InitUi();

	MainWindow->show();
}
