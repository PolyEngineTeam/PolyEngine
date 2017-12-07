#pragma once

#include "qdockwidget.h"

class EditorApp;
class PolyDockWidget;

class PolyWidget
{
public:
	PolyWidget(const QString& title, QWidget* widget);
	~PolyWidget();

	void Dock(Qt::DockWidgetArea area, QMainWindow* parent);
	void Reparent(QMainWindow* parent);

private:
	PolyDockWidget* DockWidget;
};