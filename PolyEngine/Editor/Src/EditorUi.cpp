#include "PolyEditorPCH.hpp"

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <qdockwidget.h>

void EditorUi::InitMainWindow()
{
	MainWindow = new PolyMainWindow();
	Windows.PushBack(std::move(MainWindow));

	// world explorer
	WorldExplorer = new WorldExplorerWidget();
	PolyDockWindow* window = new PolyDockWindow("World Explorer", WorldExplorer);
	DockWindows.PushBack(window);
	MainWindow->AddDockWindow(Qt::DockWidgetArea::LeftDockWidgetArea, window, true);

	// viewport
	MainViewport = new ViewportWidget();
	window = new PolyDockWindow("Viewport", MainViewport);
	DockWindows.PushBack(window);
	MainWindow->AddDockWindow(Qt::DockWidgetArea::RightDockWidgetArea, window, true);

	// main logger
	MainLogger = new LoggerWidget(eLoggerType::CONSOLE);
	window = new PolyDockWindow("Console", MainLogger);
	DockWindows.PushBack(window);
	MainWindow->AddDockWindow(Qt::DockWidgetArea::RightDockWidgetArea, window, true);
	window->hide();

	// command logger
	CmdLogger = new LoggerWidget(eLoggerType::CONSOLE);
	window = new PolyDockWindow("Cmd", CmdLogger);
	DockWindows.PushBack(window);
	MainWindow->AddDockWindow(Qt::DockWidgetArea::RightDockWidgetArea, window, true);
	window->hide();

	// entity manager
	EntityMgr = new EntityManagerWidget();
	window = new PolyDockWindow("Entity Manager", EntityMgr);
	DockWindows.PushBack(window);
	MainWindow->AddDockWindow(Qt::DockWidgetArea::RightDockWidgetArea, window, true);
	
	//EntityMgr = std::make_unique<EntityManagerWidget>("Entity Manager", MainWindow);
	//MainWindow->AddWidget(Qt::DockWidgetArea::RightDockWidgetArea, EntityMgr.get(), true);
	//
	//RTTIViewerWidget* rtti = new RTTIViewerWidget("RTTI Viewer", MainWindow);
	//MainWindow->AddWidget(Qt::DockWidgetArea::LeftDockWidgetArea, rtti, true);
	//
	//ProjectConfig* cfg = new ProjectConfig("Z:/Programming/C++/PolyEngineExamples/SGJ2017Game/SGJ2017Game.proj.json");
	//rtti->SetObject(cfg, true);
	//
	MainWindow->show();
}
