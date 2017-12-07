#pragma once

#include "qdockwidget.h"

#include "EditorApp.hpp"

class WidgetSlots : public QObject
{
	Q_OBJECT
public:
	EditorApp* App;
	QDockWidget* Widget;

public slots:
	void TopLevelChanged(bool topLevel)
	{
		if (topLevel) App->draggedWidget = Widget;
		else Poly::gConsole.LogDebug("nonFloating");
	}
};

template <class W, typename... Args>
class PolyDockWidget : public W
{
public:
	PolyDockWidget(EditorApp* app, const QString& title, Qt::DockWidgetArea area, QMainWindow* parent, Args... args) :
		W(std::forward<Args>(args)...)
	{
		DockWidget = new QDockWidget(title, parent);
		DockWidget->setWidget(this);
		parent->addDockWidget(area, DockWidget);

		Slots.App = app;
		Slots.Widget = DockWidget;

		connect(DockWidget, &QDockWidget::topLevelChanged, &Slots, &WidgetSlots::TopLevelChanged);
	}

private:
	QDockWidget* DockWidget;
	WidgetSlots Slots;
};
