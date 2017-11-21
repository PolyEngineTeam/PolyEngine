#pragma once

#include <QtWidgets/QOpenGLWidget>

#include <Engine.hpp>

class GameplayViewportWidget : public QOpenGLWidget//, public QOpenGLFunctions_3_3_CoreBackend
{
	Q_OBJECT
public:
	explicit GameplayViewportWidget(QWidget* parent = nullptr);

	void LoadEditor();
	void LoadGame(Poly::String path);

	void Update();

protected:
	void initializeGL() override;
	void paintGL() override;
	void resizeGL(int width, int height) override;

	void keyPressEvent(QKeyEvent* keyEvent) override;
	void keyReleaseEvent(QKeyEvent* keyEvent) override;

private:
	Poly::IRenderingDevice* LoadRenderingDeviceDll(HWND hwnd, RECT rect, Poly::String path);
	Poly::IGame* LoadGameDll(Poly::String path);

	Poly::Engine* Engine = nullptr;
};