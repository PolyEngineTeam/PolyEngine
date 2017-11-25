#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QDockWidget>

template <class W, typename... Args>
class PolyDockWidget : public W
{
public:
	PolyDockWidget(QMainWindow* window, const QString& dockName, Qt::DockWidgetArea area,  Args&&... args) :
		W(std::forward<Args>(args)...),
		DockWidget(new QDockWidget(window))
	{
		DockWidget->setWindowTitle(dockName);
		DockWidget->setWidget(this);
		window->addDockWidget(area, DockWidget);
	}
	~PolyDockWidget() {}

private:
	QDockWidget* DockWidget;
};