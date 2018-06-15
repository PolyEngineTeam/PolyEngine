#pragma once

#include <QtCore/qcoreevent.h>

class PolyWindow;
class PolyDockWindow;

class DockManager
{
public:
	DockManager() = default;

	void WidgetCatchEvent(PolyDockWindow* catched);
	void WidgetMoveEvent(QEvent* event);
	void WidgetDropEvent();

	void ProcessEvent(QEvent* event);

private:
	PolyWindow* MouseOver = nullptr; // over this window is currently dragged DraggedWidget
	PolyDockWindow* DraggedWidget = nullptr;
	Qt::DockWidgetArea DraggedWidgetDockArea = Qt::DockWidgetArea::TopDockWidgetArea;
};