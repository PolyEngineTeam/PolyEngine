#pragma once

#include "Windows/CustomSDLWindow.hpp"
#include "Windows/PolyDockWindow.hpp"
#include "Rendering/IRenderingDevice.hpp"

using namespace Poly;

class ViewportWidget : public PolyDockWindow
{
public:
	// Viewport widget must have parent at initialization otherwise it will appear without title bar
	explicit ViewportWidget(const QString& title, QWidget* parent);

	std::unique_ptr<IRenderingDevice> GetRenderingDevice();

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
