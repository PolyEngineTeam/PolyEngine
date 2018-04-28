#pragma once

#include <QMainWindow>

#include <Collections/Dynarray.hpp>

class PolyDockWindow;

class PolyWindow : public QMainWindow
{
public:
	PolyWindow(QWidget* parent = nullptr);

	size_t WidgetsCount() { return Widgets.GetSize(); }

	void AddWidget(Qt::DockWidgetArea area, PolyDockWindow* widget, bool isInitialization = false);
	void RemoveWidget(PolyDockWindow* widget);

private:
	Poly::Dynarray<PolyDockWindow*> Widgets;

	void closeEvent(QCloseEvent* event) override;
}; 