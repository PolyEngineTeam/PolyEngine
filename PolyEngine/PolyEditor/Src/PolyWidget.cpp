#include "PolyEditorPCH.hpp"

PolyWidget::PolyWidget(const QString& title, QWidget* parent) :
	QWidget(parent)
{
	DockWidget = new QDockWidget(title, parent);
	DockWidget->setWidget(this);

	connect(DockWidget, &QDockWidget::topLevelChanged, this, [object = this ](bool topLevel) { gApp->DockManager.WidgetCatchEvent(object);  });
}