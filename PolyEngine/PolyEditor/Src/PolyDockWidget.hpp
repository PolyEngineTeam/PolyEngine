#pragma once

#include "qdockwidget.h"

class PolyDockWidget : public QDockWidget
{
	Q_OBJECT
public:
	PolyDockWidget(const QString& title);
	~PolyDockWidget();

private slots:
	void TopLevelChanged(bool topLevel);
};
