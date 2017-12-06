#include "PolyEditorPCH.hpp"

GlobalEventFilter::GlobalEventFilter()
{
}

GlobalEventFilter::~GlobalEventFilter()
{
}

bool GlobalEventFilter::eventFilter(QObject* watched, QEvent* event)
{
	return false; // return false when you want qt to forward this event to receiver
}
