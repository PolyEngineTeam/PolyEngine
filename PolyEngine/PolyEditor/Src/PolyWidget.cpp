#include "PolyEditorPCH.hpp"

PolyWidget::PolyWidget(const QString& title, QWidget* parent) :
	QWidget(parent)
{
	DockWidget = new QDockWidget(title, parent);
	DockWidget->setWidget(this);

	connect(DockWidget, &QDockWidget::topLevelChanged, this, &PolyWidget::TopLevelChanged);
}

void PolyWidget::TopLevelChanged(bool topLevel)
{
	if (topLevel) gApp->DockManager.WidgetCatchEvent(this);
}