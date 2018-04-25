#include "PolyEditorPCH.hpp"

RTTIViewerWidget::RTTIViewerWidget(const QString& title, QWidget* parent)
	: PolyWidget(title, parent)
{
	Control = std::make_unique<RTTIViewControl>(this);
}

void RTTIViewerWidget::SetObject(RTTIBase* obj, bool debug)
{
	Control->SetObject(obj, debug);
}