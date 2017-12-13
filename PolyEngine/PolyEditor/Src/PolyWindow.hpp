#pragma once

#include <QMainWindow>

#include <Dynarray.hpp>

class PolyWidget;

class PolyWindow : public QMainWindow
{
	friend class PolyDockManager;
public:
	PolyWindow(QWidget* parent = nullptr);

	void AddWidget(Qt::DockWidgetArea area, PolyWidget* widget);
	void RemoveWidget(PolyWidget* widget);

private:
	Poly::Dynarray<PolyWidget*> Widgets;

	void closeEvent(QCloseEvent* event) override;
}; 