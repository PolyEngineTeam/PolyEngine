#pragma once

#include "qdockwidget.h"

class PolyWindow;

class PolyWidget : public QWidget
{
public:
	PolyWidget(const QString& title, QWidget* parent = nullptr);

	QDockWidget* GetDockWidget() { return DockWidget; }
	PolyWindow* GetOwner() const { return static_cast<PolyWindow*>(DockWidget->parent()); }

private:
	QDockWidget* DockWidget = nullptr;
};