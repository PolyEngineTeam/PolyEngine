#include "PolyEditorPCH.hpp"

PolyDockWindow::PolyDockWindow(const QString& title, PolyWidget* widget, QWidget* parent) :
	QDockWidget(title, parent)
{
	setWidget(widget);
	widget->setParent(this);

	Layout = new QBoxLayout(QBoxLayout::Direction::LeftToRight, this);
	Layout->addWidget(widget);

	connect(this, &QDockWidget::topLevelChanged, this, 
		[object = this](bool topLevel) { if (topLevel) gApp->DockMgr.WidgetCatchEvent(object); });
}