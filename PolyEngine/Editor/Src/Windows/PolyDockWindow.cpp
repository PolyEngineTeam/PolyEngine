#include "PolyEditorPCH.hpp"

PolyDockWindow::PolyDockWindow(const QString& title, PolyWidget* widget, QWidget* parent) :
	QDockWidget(title, parent)
{
	//widget->setParent(this);

	//setWidget(widget);
	layout()->addWidget(widget);

	connect(this, &QDockWidget::topLevelChanged, this, 
		[object = this](bool topLevel) { if (topLevel) gApp->DockMgr.WidgetCatchEvent(object); });
}