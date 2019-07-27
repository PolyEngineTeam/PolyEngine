#pragma once

#include <QMainWindow>



class PolyDockWindow;

class PolyWindow : public QMainWindow
{
public:
	PolyWindow();

	size_t DockWindowsCount() { return DockWindows.size(); }

	void AddDockWindow(Qt::DockWidgetArea area, PolyDockWindow* wnd, bool isInitialization = false);
	void RemoveDockWindow(PolyDockWindow* widget);

private:
	std::vector<PolyDockWindow*> DockWindows;

	void closeEvent(QCloseEvent* event) override;
};