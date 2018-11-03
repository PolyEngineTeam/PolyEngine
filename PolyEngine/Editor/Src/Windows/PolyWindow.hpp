#pragma once

#include <QMainWindow>

#include <Collections/Dynarray.hpp>

class PolyDockWindow;

class PolyWindow : public QMainWindow
{
public:
	PolyWindow();

	size_t DockWindowsCount() { return DockWindows.GetSize(); }

	void AddDockWindow(Qt::DockWidgetArea area, PolyDockWindow* wnd, bool isInitialization = false);
	void RemoveDockWindow(PolyDockWindow* widget);

private:
	Poly::Dynarray<PolyDockWindow*> DockWindows;

	void closeEvent(QCloseEvent* event) override;
};