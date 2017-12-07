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
	if (((EditorApp*)watched)->draggedWidget != nullptr)
		if (event->type() == QEvent::MouseButtonRelease)
		{
			PolyWindow* wnd = new PolyWindow;
			wnd->show();
			((EditorApp*)watched)->draggedWidget = nullptr;
		}

	return false; // return false when you want qt to forward this event to receiver
}
