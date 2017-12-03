#include "PolyEditorPCH.hpp"

#include <QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QHeaderView>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QDockWidget>

PolyMainWindow::PolyMainWindow(QWidget* parent) :
	PolyWindow(parent),
	Ui(this)
{
	QDockWidget* consoleDockWidget = new QDockWidget();
	PolyConsoleWidget* consoleWidget = new PolyConsoleWidget();
	consoleWidget->setFocusPolicy(Qt::ClickFocus);
	consoleWidget->setEnabled(true);
	consoleWidget->setReadOnly(true);
	consoleDockWidget->setWindowTitle("Assets Explorer");
	consoleDockWidget->setWidget(consoleWidget);
	AddWidget(consoleDockWidget, Qt::DockWidgetArea::TopDockWidgetArea, Qt::Orientation::Horizontal);

	QDockWidget* viewportDockWidget = new QDockWidget();
	// when you won't pass this argument to the constructor you won't be able to dock this widget
	PolyViewportWidget* viewportWidget = new PolyViewportWidget(viewportDockWidget); 
	viewportDockWidget->setWindowTitle("Viewport Widget");
	viewportDockWidget->setWidget(viewportWidget);
	AddWidget(viewportDockWidget, Qt::DockWidgetArea::TopDockWidgetArea, Qt::Orientation::Horizontal);

	consoleDockWidget = new QDockWidget();
	consoleWidget = new PolyConsoleWidget();
	consoleWidget->setFocusPolicy(Qt::ClickFocus);
	consoleWidget->setEnabled(true);
	consoleWidget->setReadOnly(true);
	consoleDockWidget->setWindowTitle("Object Properties");
	consoleDockWidget->setWidget(consoleWidget);
	AddWidget(consoleDockWidget, Qt::DockWidgetArea::TopDockWidgetArea, Qt::Orientation::Horizontal);

	consoleDockWidget = new QDockWidget();
	consoleWidget = new PolyConsoleWidget();
	consoleWidget->setFocusPolicy(Qt::ClickFocus);
	consoleWidget->setEnabled(true);
	consoleWidget->setReadOnly(true);
	consoleDockWidget->setWindowTitle("Console");
	consoleDockWidget->setWidget(consoleWidget);
	AddWidget(consoleDockWidget, Qt::DockWidgetArea::BottomDockWidgetArea, Qt::Orientation::Horizontal);
}

PolyMainWindow::~PolyMainWindow()
{
}

void PolyMainWindow::CloseProject()
{
}

void PolyMainWindow::Quit()
{
}

void PolyMainWindow::Undo()
{
}

void PolyMainWindow::Redo()
{
}

void PolyMainWindow::AddWindow()
{
}

void PolyMainWindow::Build()
{
}

void PolyMainWindow::ContactUs()
{
}
