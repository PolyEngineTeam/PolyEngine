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
	void resizeEvent(QResizeEvent* resizeEvent) override;
	void keyPressEvent(QKeyEvent* keyEvent) override;
	void keyReleaseEvent(QKeyEvent* keyEvent) override;

private:
	std::unique_ptr<Poly::Engine> Engine = nullptr;
};