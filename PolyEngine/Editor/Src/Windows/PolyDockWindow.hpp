#pragma once

#include <QDockWidget>

class PolyWindow;

class PolyDockWindow : public QWidget
{
public:
	PolyDockWindow(const QString& title, QWidget* parent = nullptr);

	QDockWidget* GetDockWidget() { return DockWidget; }
	PolyWindow* GetOwner() const { return static_cast<PolyWindow*>(DockWidget->parent()); }

private:
	QDockWidget* DockWidget = nullptr;
};