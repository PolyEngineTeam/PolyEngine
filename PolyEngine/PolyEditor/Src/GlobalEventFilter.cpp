#include "PolyEditorPCH.hpp"

GlobalEventFilter::GlobalEventFilter()
{
}

GlobalEventFilter::~GlobalEventFilter()
{
}

bool GlobalEventFilter::eventFilter(QObject* watched, QEvent* event)
{
	gApp->DockManager.ProcessEvent(event);

	return false; // you should return false when you want qt to forward this event to receiver
}
