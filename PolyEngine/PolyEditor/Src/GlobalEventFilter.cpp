#include "PolyEditorPCH.hpp"

bool GlobalEventFilter::eventFilter(QObject* watched, QEvent* event)
{
	UNUSED(watched);
	gApp->DockManager.ProcessEvent(event);
	return false; // you should return false when you want qt to forward this event to receiver
}
