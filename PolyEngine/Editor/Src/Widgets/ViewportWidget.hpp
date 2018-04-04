#pragma once

#include "Windows/CustomSDLWindow.hpp"
#include "PolyWidget.hpp"
#include "Rendering/IRenderingDevice.hpp"

class ViewportWidget : public PolyWidget
{
public:
	// Viewport widget must have parent at initialization otherwise it will appear without title bar
	explicit ViewportWidget(const QString& title, QWidget* parent);

	std::unique_ptr<Poly::IRenderingDevice> InitializeViewport();

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
