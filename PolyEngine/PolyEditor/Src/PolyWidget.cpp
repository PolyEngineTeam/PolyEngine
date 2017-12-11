#include "PolyEditorPCH.hpp"

PolyWidget::PolyWidget(const QString& title, QWidget* parent)
	: QWidget(parent)
{
	DockWidget = new PolyDockWidget(title);
	DockWidget->setWidget(this);
}

PolyWidget::~PolyWidget()
{
}

void PolyWidget::Dock(Qt::DockWidgetArea area, QMainWindow* parent)
{
	DockWidget->setParent(parent);
	parent->addDockWidget(area, DockWidget);
}

void PolyWidget::Reparent(QMainWindow* parent)
{
	DockWidget->setParent(parent);
}
