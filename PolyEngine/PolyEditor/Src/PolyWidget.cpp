#include "PolyEditorPCH.hpp"

PolyWidget::PolyWidget(const QString& title, QWidget* parent) :
	QWidget(parent),
	Title(title)
{
}

PolyWidget::~PolyWidget()
{
}

void PolyWidget::Connect()
{
	connect(DockWidget, &QDockWidget::topLevelChanged, this, &PolyWidget::TopLevelChanged);
}

void PolyWidget::TopLevelChanged(bool topLevel)
{
	if (topLevel) gApp->DockManager.WidgetCatchEvent(this);
}
