#pragma once

#include <qevent.h>

class PolyWindow;
class PolyWidget;

class PolyDockManager
{
	friend class GlobalEventFilter;
public:
	PolyDockManager() = default;

	void WidgetCatchEvent(PolyWidget* catched);
	void ProcessEvent(QEvent* event);

private:
	PolyWindow* MouseOver = nullptr;
	PolyWidget* DraggedWidget = nullptr;

	void WidgetMoveEvent(QEvent* event);
	void WidgetDropEvent(QEvent* event);
};