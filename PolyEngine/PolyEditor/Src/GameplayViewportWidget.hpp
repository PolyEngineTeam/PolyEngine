#pragma once

#include <QtWidgets/QOpenGLWidget>

#include <Engine.hpp>

class GameplayViewportWidget : public QOpenGLWidget//, public QOpenGLFunctions_3_3_CoreBackend
{
	Q_OBJECT
public:
	explicit GameplayViewportWidget(QWidget* parent = nullptr);

protected:
	void initializeGL() override;
	void paintGL() override;
	void resizeGL(int width, int height) override;

	Poly::IRenderingDevice* LoadRenderingDevice(HWND hwnd, RECT rect);
	Poly::IGame* LoadGame();

	Poly::Engine* Engine;
};