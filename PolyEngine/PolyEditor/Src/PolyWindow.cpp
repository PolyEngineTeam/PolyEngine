#include "PolyEditorPCH.hpp"

#include <QDockWidget>

PolyWindow::PolyWindow(QWidget* parent) :
	QMainWindow(parent)
{
}

PolyWindow::~PolyWindow()
{
}

void PolyWindow::AddWidget(Qt::DockWidgetArea area, PolyWidget* widget)
{
	if (widget->DockWidget)
		((PolyWindow*)widget->DockWidget->parent())->RemoveWidget(widget);

	widget->DockWidget = new QDockWidget(widget->Title, this);
	widget->DockWidget->setWidget(widget);
	addDockWidget(area, widget->DockWidget);
	widget->Connect();

	Widgets.PushBack(widget);
}

void PolyWindow::RemoveWidget(PolyWidget* widget)
{
	Widgets.Remove(widget);
	widget->DockWidget = nullptr;
}
