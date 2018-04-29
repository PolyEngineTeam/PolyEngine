#include "PolyEditorPCH.hpp"

PolyDockWindow::PolyDockWindow(const QString& title, PolyWidget* widget) :
	QDockWidget(title)
{
	widget->setParent(this);

	setWidget(widget);
	
	// for now use this to create viewport properly
	//layout()->addWidget(widget);

	connect(this, &QDockWidget::topLevelChanged, this, 
		[object = this](bool topLevel) { if (topLevel) gApp->DockMgr.WidgetCatchEvent(object); });
}