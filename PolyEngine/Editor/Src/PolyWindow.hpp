#pragma once

#include <QMainWindow>

#include <Dynarray.hpp>

class PolyWidget;

class PolyWindow : public QMainWindow
{
public:
	PolyWindow(QWidget* parent = nullptr);

	size_t WidgetsCount() { return Widgets.GetSize(); }

	void AddWidget(Qt::DockWidgetArea area, PolyWidget* widget, bool isInitialization = false);
	void RemoveWidget(PolyWidget* widget);

private:
	Poly::Dynarray<PolyWidget*> Widgets;

	void closeEvent(QCloseEvent* event) override;
}; 