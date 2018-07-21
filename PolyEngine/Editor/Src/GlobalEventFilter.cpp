#include "PolyEditorPCH.hpp"

bool GlobalEventFilter::eventFilter(QObject* watched, QEvent* event)
{
	UNUSED(watched);
	gApp->DockMgr->ProcessEvent(event);
	gApp->CommandMgr->ProcessEvent(event);
	return false; // you should return false when you want qt to forward this event to receiver
}
