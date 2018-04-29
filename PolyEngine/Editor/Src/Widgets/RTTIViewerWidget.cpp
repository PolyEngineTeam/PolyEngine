#include "PolyEditorPCH.hpp"

RTTIViewerWidget::RTTIViewerWidget()
{
	Control = std::make_unique<RTTIViewControl>(this);
}

void RTTIViewerWidget::SetObject(RTTIBase* obj, bool debug)
{
	Control->SetObject(obj, debug);
}