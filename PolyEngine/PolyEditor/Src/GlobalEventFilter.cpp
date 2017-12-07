#include "PolyEditorPCH.hpp"

GlobalEventFilter::GlobalEventFilter(PolyEditorUi* ui)
{
	Ui = ui;
}

GlobalEventFilter::~GlobalEventFilter()
{
}

bool GlobalEventFilter::eventFilter(QObject* watched, QEvent* event)
{
	if (gApp->draggedWidget != nullptr)
		if (event->type() == QEvent::MouseButtonRelease)
		{
			PolyWindow* wnd = new PolyWindow;
			wnd->resize(gApp->draggedWidget->size());
			wnd->move(gApp->draggedWidget->pos());
			wnd->AddWidget(gApp->draggedWidget, Qt::DockWidgetArea::TopDockWidgetArea);
			wnd->show();
			gApp->draggedWidget = nullptr;
		}

	return false; // return false when you want qt to forward this event to receiver
}
