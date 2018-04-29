#include "PolyEditorPCH.hpp"
#include <Engine.hpp>
#include <Utils/LibraryLoader.hpp>
#include <Configs\AssetsPathConfig.hpp>
#include <SDL.h>

extern "C"
{
	using CreateRenderingDeviceFunc = Poly::IRenderingDevice* (SDL_Window*, const Poly::ScreenSize&);
}

static Poly::LibraryFunctionHandle<CreateRenderingDeviceFunc> LoadRenderingDevice;

// ---------------------------------------------------------------------------------------------------------
ViewportWidget::ViewportWidget()
{
	setAttribute(Qt::WA_NativeWindow);
	setMouseTracking(true);
	setFocusPolicy(Qt::ClickFocus);
	setMinimumSize(320, 200);

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		ASSERTE(false, "SDL initialization failed!");
	}
	Poly::gConsole.LogDebug("SDL initialized.");

	// TODO: catch winId changes (http://doc.qt.io/qt-5/qwidget.html#winId)
	// TODO: something like addviewport to rendering device
	ASSERTE(!WindowInSDL.IsValid(), "Window already initialized!");
	WindowInSDL = CustomSDLWindow::CreateSDLWindowFromArgs((void*)winId(), SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	ASSERTE(WindowInSDL.IsValid(), "Window creation failed!");

}

// ---------------------------------------------------------------------------------------------------------
std::unique_ptr<Poly::IRenderingDevice> ViewportWidget::GetRenderingDevice()
{
	// TODO fix library names differences between platforms
	if (!LoadRenderingDevice.FunctionValid())
	{
		// Load rendering device library
		LoadRenderingDevice = Poly::LoadFunctionFromSharedLibrary<CreateRenderingDeviceFunc>(
			gApp->ProjectMgr.GetProjectConfig().GetRenderingDeviceDllPath().GetCStr(), "PolyCreateRenderingDevice");
		ASSERTE(LoadRenderingDevice.FunctionValid(), "Library libRenderingDevice load failed");
		Poly::gConsole.LogDebug("Library libRenderingDevice loaded.");
	}

	Poly::ScreenSize viewportRect;
	viewportRect.Width = width();
	viewportRect.Height = height();

	return std::unique_ptr<Poly::IRenderingDevice>(LoadRenderingDevice(WindowInSDL.Get(), viewportRect));
}

// ---------------------------------------------------------------------------------------------------------
void ViewportWidget::resizeEvent(QResizeEvent* resizeEvent)
{
	if (gApp->EngineMgr.GetEngineState() == eEngineState::NONE)
		return;

	Poly::ScreenSize screenSize;
	screenSize.Width = resizeEvent->size().width();
	screenSize.Height = resizeEvent->size().height();
	Poly::gEngine->ResizeScreen(screenSize);
}

// ---------------------------------------------------------------------------------------------------------
void ViewportWidget::wheelEvent(QWheelEvent* wheelEvent)
{
	if (gApp->EngineMgr.GetEngineState() == eEngineState::NONE)
		return;
	Poly::gEngine->UpdateWheelPos(Poly::Vector2i(wheelEvent->delta(), 0));
}

// ---------------------------------------------------------------------------------------------------------
void ViewportWidget::mouseMoveEvent(QMouseEvent* mouseEvent)
{
	if (gApp->EngineMgr.GetEngineState() == eEngineState::NONE)
		return;
	Poly::gEngine->UpdateMousePos(Poly::Vector2i(mouseEvent->pos().x(), mouseEvent->pos().y()));
}

// ---------------------------------------------------------------------------------------------------------
void ViewportWidget::mousePressEvent(QMouseEvent* mouseEvent)
{
	if (gApp->EngineMgr.GetEngineState() == eEngineState::NONE)
		return;

	switch (mouseEvent->button())
	{
	case Qt::LeftButton:
		Poly::gEngine->MouseButtonDown(static_cast<Poly::eMouseButton>(Poly::eMouseButton::LEFT));
		break;

	case Qt::RightButton:
		Poly::gEngine->MouseButtonDown(static_cast<Poly::eMouseButton>(Poly::eMouseButton::RIGHT));
		break;

	case Qt::MiddleButton:
		Poly::gEngine->MouseButtonDown(static_cast<Poly::eMouseButton>(Poly::eMouseButton::MIDDLE));
		break;

	default:
		// unsupported key
		break;
	}
}

// ---------------------------------------------------------------------------------------------------------
void ViewportWidget::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
	if (gApp->EngineMgr.GetEngineState() == eEngineState::NONE)
		return;
	switch (mouseEvent->button())
	{
	case Qt::LeftButton:
		Poly::gEngine->MouseButtonUp(static_cast<Poly::eMouseButton>(Poly::eMouseButton::LEFT));
		break;

	case Qt::RightButton:
		Poly::gEngine->MouseButtonUp(static_cast<Poly::eMouseButton>(Poly::eMouseButton::RIGHT));
		break;

	case Qt::MiddleButton:
		Poly::gEngine->MouseButtonUp(static_cast<Poly::eMouseButton>(Poly::eMouseButton::MIDDLE));
		break;

	default:
		// unsupported key
		break;
	}
}

// ---------------------------------------------------------------------------------------------------------
void ViewportWidget::keyPressEvent(QKeyEvent* keyEvent)
{
	if (gApp->EngineMgr.GetEngineState() == eEngineState::NONE)
		return;

	if (keyEvent->isAutoRepeat())
		keyEvent->ignore();
	else
		Poly::gEngine->KeyDown(static_cast<Poly::eKey>(SDL_GetScancodeFromKey(QtKeyEventToSDLKeycode((Qt::Key)keyEvent->key()))));
}

// ---------------------------------------------------------------------------------------------------------
void ViewportWidget::keyReleaseEvent(QKeyEvent* keyEvent)
{
	if (gApp->EngineMgr.GetEngineState() == eEngineState::NONE)
		return;

	if (keyEvent->isAutoRepeat())
		keyEvent->ignore();
	else
 		Poly::gEngine->KeyUp(static_cast<Poly::eKey>(SDL_GetScancodeFromKey(QtKeyEventToSDLKeycode((Qt::Key)keyEvent->key()))));
}