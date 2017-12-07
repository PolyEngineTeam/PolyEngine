#include "PolyEditorPCH.hpp"

PolyDockWidget::PolyDockWidget(const QString& title) :
	QDockWidget(title)
{
	connect(this, &QDockWidget::topLevelChanged, this, &PolyDockWidget::TopLevelChanged);
}

PolyDockWidget::~PolyDockWidget()
{
}

void PolyDockWidget::TopLevelChanged(bool topLevel)
{
	if (topLevel) gApp->draggedWidget = this;
	//else Poly::gConsole.LogDebug("nonFloating");
}
