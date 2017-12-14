#pragma once

#include "qdockwidget.h"

class PolyWindow;

class PolyWidget : public QWidget
{
	Q_OBJECT
public:
	PolyWidget(const QString& title, QWidget* parent);

	QDockWidget* GetDockWidget() { return DockWidget; }
	PolyWindow* GetOwner() const { return static_cast<PolyWindow*>(DockWidget->parent()); }

private:
	QString Title;
	QDockWidget* DockWidget = nullptr;

private slots:
	void TopLevelChanged(bool topLevel);
};