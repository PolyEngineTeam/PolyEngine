#pragma once

#include <QtWidgets/QOpenGLWidget>

#include <Engine.hpp>

class GameplayViewportWidget : public QOpenGLWidget, protected QOpenGLFunctions//, public QOpenGLFunctions_3_3_CoreBackend
{
	Q_OBJECT
public:
	explicit GameplayViewportWidget(QWidget* parent = nullptr);

	void LoadEditor();

	void Update();

protected:
	void initializeGL() override;
	void paintGL() override;
	void resizeGL(int width, int height) override;

	void keyPressEvent(QKeyEvent* keyEvent) override;
	void keyReleaseEvent(QKeyEvent* keyEvent) override;

private:
	std::unique_ptr<Poly::Engine> Engine = nullptr;
};