#include "PolyEditorPCH.hpp"
#include "GameplayViewportWidget.hpp"

#include <windows.h>
#include <windowsx.h>

// ---------------------------------------------------------------------------------------------------------
GameplayViewportWidget::GameplayViewportWidget(QWidget* parent)
	: QOpenGLWidget(parent)
{
}

void GameplayViewportWidget::Init(std::unique_ptr<Poly::IGame> game, std::unique_ptr<Poly::IRenderingDevice> device)
{
	Engine.Init(std::move(game), std::move(device));
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::Update()
{
	Engine.Update();
}

//-------------------------------------------------------------------------------------------------------- -
RECT GameplayViewportWidget::GetRect()
{
	RECT viewportRect;
	viewportRect.top = 0;
	viewportRect.left = 0;
	viewportRect.right = width();
	viewportRect.bottom = height();

	return viewportRect;
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::initializeGL()
{
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::paintGL()
{
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::resizeGL(int w, int h)
{
	Poly::ScreenSize screenSize;
	screenSize.Width = width();
	screenSize.Height = height();
	Engine.ResizeScreen(screenSize);
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::keyPressEvent(QKeyEvent* keyEvent)
{
	Engine.KeyDown(static_cast<Poly::eKey>((unsigned int)keyEvent->nativeVirtualKey()));
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::keyReleaseEvent(QKeyEvent* keyEvent)
{
	if (keyEvent->isAutoRepeat())
		keyEvent->ignore();
	else
 		Engine.KeyUp(static_cast<Poly::eKey>((unsigned int)keyEvent->nativeVirtualKey()));
}