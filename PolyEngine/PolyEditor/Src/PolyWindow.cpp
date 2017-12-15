#include "PolyEditorPCH.hpp"

#include <QDockWidget>

PolyWindow::PolyWindow(QWidget* parent) :
	QMainWindow(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
}

void PolyWindow::AddWidget(Qt::DockWidgetArea area, PolyWidget* widget)
{
	if(PolyWindow* window = dynamic_cast<PolyWindow*>(widget->parent()))
		window->RemoveWidget(widget);

	addDockWidget(area, widget->GetDockWidget());
	QTimer::singleShot(1, this, [a = area,  w = widget, object = this]() { object->addDockWidget(a, w->GetDockWidget()); });
	Widgets.PushBack(widget);
}

void PolyWindow::RemoveWidget(PolyWidget* widget)
{
	Widgets.Remove(widget);
	removeDockWidget(widget->GetDockWidget());
}

void PolyWindow::closeEvent(QCloseEvent* event)
{
	gApp->Ui.Windows.Remove(this);
}
