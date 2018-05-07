#include "PolyEditorPCH.hpp"

#include <QDockWidget>

PolyWindow::PolyWindow(QWidget* parent) :
	QMainWindow(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
}

void PolyWindow::AddWidget(Qt::DockWidgetArea area, PolyDockWindow* widget, bool isInitialization)
{
	if(PolyWindow* window = dynamic_cast<PolyWindow*>(widget->parent()))
		window->RemoveWidget(widget);
	
	if(isInitialization)
		addDockWidget(area, widget);
	else
		QTimer::singleShot(1, this, [a = area, w = widget, object = this]() { object->addDockWidget(a, w); w->show(); });

	Widgets.PushBack(widget);
}

void PolyWindow::RemoveWidget(PolyDockWindow* widget)
{
	Widgets.Remove(widget);
	removeDockWidget(widget);
}

void PolyWindow::closeEvent(QCloseEvent* event)
{
	UNUSED(event);
	gApp->Ui.Windows.Remove(this);
}