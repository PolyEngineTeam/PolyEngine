#pragma once

#include <QMainWindow>

#include <Dynarray.hpp>

class PolyWidget;

class PolyWindow : public QMainWindow
{
public:
	PolyWindow(QWidget* parent = nullptr);

	int WidgetsCount() { return Widgets.GetSize(); }

	void AddWidget(Qt::DockWidgetArea area, PolyWidget* widget);
	void RemoveWidget(PolyWidget* widget);

private:
	Poly::Dynarray<PolyWidget*> Widgets;

	void closeEvent(QCloseEvent* event) override;
}; 