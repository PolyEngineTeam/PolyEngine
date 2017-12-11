#pragma once

#include "qdockwidget.h"
#include "qwidget.h"

class EditorApp;
class PolyDockWidget;

class PolyWidget: public QWidget
{
public:
	PolyWidget(const QString& title, QWidget* parent);
	~PolyWidget();

	void Dock(Qt::DockWidgetArea area, QMainWindow* parent);
	void Reparent(QMainWindow* parent);

private:
	PolyDockWidget* DockWidget;
};