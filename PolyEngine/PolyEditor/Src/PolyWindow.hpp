#pragma once

#include <QMainWindow>

#include <Dynarray.hpp>

class PolyWindow : public QMainWindow
{
public:
	PolyWindow(QWidget* parent = nullptr);
	~PolyWindow();

	void AddWidget(QDockWidget* widget, Qt::DockWidgetArea area, Qt::Orientation orientation);

private:
	Poly::Dynarray<QWidget> Widgets;
};