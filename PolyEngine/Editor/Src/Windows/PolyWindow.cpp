#include "PolyEditorPCH.hpp"

#include <QDockWidget>

PolyWindow::PolyWindow()
{
	setAttribute(Qt::WA_DeleteOnClose);
	setDockOptions(QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks);
}

void PolyWindow::AddDockWindow(Qt::DockWidgetArea area, PolyDockWindow* wnd, bool isInitialization)
{
	if(PolyWindow* window = dynamic_cast<PolyWindow*>(wnd->parent()))
		window->RemoveDockWindow(wnd);
	
	if(isInitialization)
		addDockWidget(area, wnd);
	else
		QTimer::singleShot(1, this, [a = area, w = wnd, object = this]() 
			{ object->addDockWidget(a, w); w->show(); });

	DockWindows.PushBack(wnd);
}

void PolyWindow::RemoveDockWindow(PolyDockWindow* wnd)
{
	DockWindows.Remove(wnd);
	removeDockWidget(wnd);
}

void PolyWindow::closeEvent(QCloseEvent* event)
{
	UNUSED(event);

	Poly::Dynarray<PolyDockWindow*> windows = DockWindows;

	for (auto wnd : windows)
		gApp->Ui.MainWindow->AddDockWindow(Qt::DockWidgetArea::LeftDockWidgetArea, wnd);

	gApp->Ui.Windows.Remove(this);
}
