#pragma once

#include "qdockwidget.h"

class PolyWidget : public QWidget
{
	friend class PolyWindow;

	Q_OBJECT
public:
	PolyWidget(const QString& title, QWidget* parent);

	QDockWidget* GetDockWidget() const { return DockWidget; }
	PolyWindow* GetOwner() const { return Owner; }

private:
	void Connect();

	QString Title;
	PolyWindow* Owner = nullptr;
	QDockWidget* DockWidget = nullptr;

private slots:
	void TopLevelChanged(bool topLevel);
};