#include "PolyEditorPCH.hpp"

#include <QDockWidget>

PolyWindow::PolyWindow(QWidget* parent) :
	QMainWindow(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
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
	widget->Owner = this;
}

void PolyWindow::RemoveWidget(PolyWidget* widget)
{
	Widgets.Remove(widget);
	widget->DockWidget->setWidget(nullptr);
	((PolyWindow*)widget->DockWidget->parent())->removeDockWidget(widget->DockWidget);
	widget->DockWidget = nullptr;
}

void PolyWindow::closeEvent(QCloseEvent* event)
{
	gApp->Ui.Windows.Remove(this);
}
