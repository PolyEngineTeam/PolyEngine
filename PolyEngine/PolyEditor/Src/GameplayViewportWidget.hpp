#pragma once

#include <QtWidgets/QOpenGLWidget>

#include <Engine.hpp>

namespace Poly
{
	class IGame;
	class IRenderingDevice;
}

class GameplayViewportWidget : public QOpenGLWidget//, public QOpenGLFunctions_3_3_CoreBackend
{
	Q_OBJECT
public:
	explicit GameplayViewportWidget(QWidget* parent = nullptr);

	void Init(std::unique_ptr<Poly::IGame> game, std::unique_ptr<Poly::IRenderingDevice> device);
	void Update();

	HWND GetHwnd() { return (HWND)effectiveWinId(); }
	RECT GetRect();

protected:
	void initializeGL() override;
	void paintGL() override;
	void resizeGL(int width, int height) override;

	void keyPressEvent(QKeyEvent* keyEvent) override;
	void keyReleaseEvent(QKeyEvent* keyEvent) override;

private:
	Poly::Engine Engine;
};