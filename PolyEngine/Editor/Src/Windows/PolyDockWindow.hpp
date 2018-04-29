#pragma once

#include <QDockWidget>

class PolyWindow;
class PolyWidget;

class PolyDockWindow : public QDockWidget
{
public:
	PolyDockWindow(const QString& title, PolyWidget* widget, QWidget* parent = nullptr);

	PolyWindow* GetOwner() const { return static_cast<PolyWindow*>(parent()); }
	//PolyWidget* GetContent() const { return static_cast<PolyWidget*>(Layout->children()[0]); }
};