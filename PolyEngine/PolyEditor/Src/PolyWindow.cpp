#include "PolyEditorPCH.hpp"

#include <QDockWidget>

PolyWindow::PolyWindow(QWidget* parent) :
	QMainWindow(parent)
{
}

PolyWindow::~PolyWindow()
{
}

void PolyWindow::AddWidget(QDockWidget* widget, Qt::DockWidgetArea area, Qt::Orientation orientation)
{
	UNUSED(orientation);
	//Widgets.PushBack(*widget);
	addDockWidget(area, widget);
}
