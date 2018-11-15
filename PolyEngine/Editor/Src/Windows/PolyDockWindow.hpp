#pragma once

#include <QDockWidget>

class PolyWindow;
class PolyWidget;

class PolyDockWindow : public QDockWidget
{
public:
	PolyDockWindow(const QString& title, PolyWidget* widget);

	PolyWindow* GetOwner() const { return static_cast<PolyWindow*>(parent()); }

protected:
	void paintEvent(QPaintEvent *e) override;

private:
	QString Title;
};