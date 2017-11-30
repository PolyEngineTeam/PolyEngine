#pragma once

#include <QWidget>
#include <QtGui/QtGui>
#include "CustomSDLWindow.hpp"

class PolyViewportWidget : public QWidget
{
public:
	explicit PolyViewportWidget(QWidget* parent = nullptr);

	void InitializeViewport();

protected:
	// overriden events
	void resizeEvent(QResizeEvent* resizeEvent) override;
	void wheelEvent(QWheelEvent* wheelEvent) override;
	void mouseMoveEvent(QMouseEvent* mouseEvent) override;
	void mousePressEvent(QMouseEvent* mouseEvent) override;
	void mouseReleaseEvent(QMouseEvent* mouseREvent) override;
	void keyPressEvent(QKeyEvent* keyEvent) override;
	void keyReleaseEvent(QKeyEvent* keyEvent) override;

private:
	CustomSDLWindow WindowInSDL;
};