#include "PolyEditorPCH.hpp"

#include <QDockWidget>

PolyWindow::PolyWindow(QWidget* parent) :
	QMainWindow(parent)
{
}

PolyWindow::~PolyWindow()
{
}

void PolyWindow::AddWidget(QDockWidget* widget, Qt::DockWidgetArea area)
{
	//Widgets.PushBack(*widget);
	widget->setParent(this);
	addDockWidget(area, widget);
}
