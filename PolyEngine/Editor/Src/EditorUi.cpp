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

	// world explorer
	WorldExplorer = new WorldInspectorWidget(MainWindow);
	PolyDockWindow* window = new PolyDockWindow("World Inspector", WorldExplorer);
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
	EntityMgr = new EntityInspectorWidget(MainWindow);
	window = new PolyDockWindow("Entity Inspector", EntityMgr);
	DockWindows.PushBack(window);
	MainWindow->AddDockWindow(Qt::DockWidgetArea::RightDockWidgetArea, window, true);

	// resource manager
	//ResourceMgr = new ResourceInspectorWidget();
	//window = new PolyDockWindow("Resource Manager", ResourceMgr);
	//DockWindows.PushBack(window);
	//MainWindow->AddDockWindow(Qt::DockWidgetArea::BottomDockWidgetArea, window, true);

	MainWindow->show();
}
