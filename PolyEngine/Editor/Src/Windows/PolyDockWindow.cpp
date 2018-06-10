#include "PolyEditorPCH.hpp"

PolyDockWindow::PolyDockWindow(const QString& title, PolyWidget* widget) :
	QDockWidget(title)
{
	widget->setParent(this);

	setWidget(widget);
	
	connect(this, &QDockWidget::topLevelChanged, this, 
		[object = this](bool topLevel) { if (topLevel) gApp->DockMgr->WidgetCatchEvent(object); });
}