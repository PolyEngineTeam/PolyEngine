#include "PolyEditorPCH.hpp"
#include <Engine.hpp>
#include <LibraryLoader.hpp>
#include <AssetsPathConfig.hpp>
#include <SDL.h>

extern "C"
{
	using CreateRenderingDeviceFunc = Poly::IRenderingDevice* (SDL_Window*, const Poly::ScreenSize&);
	using CreateGameFunc = Poly::IGame* (void);
}

static Poly::LibraryFunctionHandle<CreateRenderingDeviceFunc> LoadRenderingDevice;
static Poly::LibraryFunctionHandle<CreateGameFunc> LoadGame;


// ---------------------------------------------------------------------------------------------------------
ViewportWidget::ViewportWidget(const QString& title, QWidget* parent) :
	PolyWidget(title, parent)
{
	setAttribute(Qt::WA_NativeWindow);
	setMouseTracking(true);
	setFocusPolicy(Qt::ClickFocus);
	setMinimumSize(320, 200);

	connect(gApp, &EditorApp::EngineCreated, [this]() { InitializeViewport(); });
}

// ---------------------------------------------------------------------------------------------------------
void ViewportWidget::InitializeViewport()
{
	// TODO fix library names differences between platforms
	if (!LoadRenderingDevice.FunctionValid())
	{
		// Load rendering device library
		LoadRenderingDevice = Poly::LoadFunctionFromSharedLibrary<CreateRenderingDeviceFunc>(Poly::gAssetsPathConfig.GetRenderingDeviceLibPath().GetCStr(), "PolyCreateRenderingDevice");
		ASSERTE(LoadRenderingDevice.FunctionValid(), "Library libRenderingDevice load failed");
		Poly::gConsole.LogDebug("Library libRenderingDevice loaded.");
	}
	
	if (!LoadGame.FunctionValid())
	{
		// Load game library
		LoadGame = Poly::LoadFunctionFromSharedLibrary<CreateGameFunc>(Poly::gAssetsPathConfig.GetGameLibPath().GetCStr(), "CreateGame");
		ASSERTE(LoadGame.FunctionValid(), "Library libGame load failed");
		Poly::gConsole.LogDebug("Library libGame loaded.");
	}
	
	
	Poly::ScreenSize viewportRect;
	viewportRect.Width = width();
	viewportRect.Height = height();

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		ASSERTE(false, "SDL initialization failed!");
	}
	Poly::gConsole.LogDebug("SDL initialized.");


	// TODO: catch winId changes (http://doc.qt.io/qt-5/qwidget.html#winId)
	// TODO: something like addviewport to rendering device
	std::unique_ptr<Poly::IGame> game = std::unique_ptr<Poly::IGame>(LoadGame());
	ASSERTE(!WindowInSDL.IsValid(), "Window already initialized!");
	WindowInSDL = CustomSDLWindow::CreateSDLWindowFromArgs((void*)winId(), SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	ASSERTE(WindowInSDL.IsValid(), "Window creation failed!");
	std::unique_ptr<Poly::IRenderingDevice> device = std::unique_ptr<Poly::IRenderingDevice>(LoadRenderingDevice(WindowInSDL.Get(), viewportRect));
	Poly::gEngine->Init(std::move(game), std::move(device));
	Poly::gConsole.LogDebug("Engine loaded successfully");
}

// ---------------------------------------------------------------------------------------------------------
void ViewportWidget::resizeEvent(QResizeEvent* resizeEvent)
{
	if (!Poly::gEngine)
		return;

	Poly::ScreenSize screenSize;
	screenSize.Width = resizeEvent->size().width();
	screenSize.Height = resizeEvent->size().height();
	Poly::gEngine->ResizeScreen(screenSize);
}

// ---------------------------------------------------------------------------------------------------------
void ViewportWidget::wheelEvent(QWheelEvent* wheelEvent)
{
	if (!Poly::gEngine)
		return;
	Poly::gEngine->UpdateWheelPos(Poly::Vector2i(wheelEvent->delta(), 0));
}

// ---------------------------------------------------------------------------------------------------------
void ViewportWidget::mouseMoveEvent(QMouseEvent* mouseEvent)
{
	if (!Poly::gEngine)
		return;
	Poly::gEngine->UpdateMousePos(Poly::Vector2i(mouseEvent->pos().x(), mouseEvent->pos().y()));
}

// ---------------------------------------------------------------------------------------------------------
void ViewportWidget::mousePressEvent(QMouseEvent* mouseEvent)
{
	if (!Poly::gEngine)
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
	if (!Poly::gEngine)
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
	if (!Poly::gEngine)
		return;

	if (keyEvent->isAutoRepeat())
		keyEvent->ignore();
	else
		Poly::gEngine->KeyDown(static_cast<Poly::eKey>(SDL_GetScancodeFromKey(QtKeyEventToSDLKeycode((Qt::Key)keyEvent->key()))));
}

// ---------------------------------------------------------------------------------------------------------
void ViewportWidget::keyReleaseEvent(QKeyEvent* keyEvent)
{
	if (!Poly::gEngine)
		return;

	if (keyEvent->isAutoRepeat())
		keyEvent->ignore();
	else
 		Poly::gEngine->KeyUp(static_cast<Poly::eKey>(SDL_GetScancodeFromKey(QtKeyEventToSDLKeycode((Qt::Key)keyEvent->key()))));
}