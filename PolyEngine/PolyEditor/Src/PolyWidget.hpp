#pragma once

#include "qdockwidget.h"

class PolyWidget : public QWidget
{
	friend class PolyWindow;

	Q_OBJECT
public:
	PolyWidget(const QString& title, QWidget* parent);
	~PolyWidget();

	const QDockWidget* GetDockWidget() const { return DockWidget; }

private:
	void Connect();

	QString Title;
	QDockWidget* DockWidget = nullptr;

private slots:
	void TopLevelChanged(bool topLevel);
};