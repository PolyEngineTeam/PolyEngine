#pragma once

#include <QtWidgets/QOpenGLWidget>

#include <Engine.hpp>

class GameplayViewportWidget : public QWidget
{
	Q_OBJECT
public:
	explicit GameplayViewportWidget(QWidget* parent = nullptr);

	void LoadEditor();

	void Update();

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
	std::unique_ptr<Poly::Engine> Engine = nullptr;
};