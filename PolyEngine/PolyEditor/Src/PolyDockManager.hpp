#pragma once

#include <qevent.h>

class PolyWindow;
class PolyWidget;

class PolyDockManager
{
public:
	PolyDockManager() = default;

	void WidgetCatchEvent(PolyWidget* catched);
	void WidgetMoveEvent(QEvent* event);
	void WidgetDropEvent();

	void ProcessEvent(QEvent* event);

private:
	PolyWindow* MouseOver = nullptr; // over this window is currently dragged DraggedWidget
	PolyWidget* DraggedWidget = nullptr;
	Qt::DockWidgetArea DraggedWidgetDockArea = Qt::DockWidgetArea::TopDockWidgetArea;
};