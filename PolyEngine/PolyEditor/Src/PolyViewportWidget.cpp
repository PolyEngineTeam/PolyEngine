#include "PolyEditorPCH.hpp"
#include <Engine.hpp>
#include <LibraryLoader.hpp>
#include <SDL.h>

extern "C"
{
	using CreateRenderingDeviceFunc = Poly::IRenderingDevice* (SDL_Window*, const Poly::ScreenSize&);
	using CreateGameFunc = Poly::IGame* (void);
}

static Poly::LibraryFunctionHandle<CreateRenderingDeviceFunc> LoadRenderingDevice;
static Poly::LibraryFunctionHandle<CreateGameFunc> LoadGame;


// ---------------------------------------------------------------------------------------------------------
PolyViewportWidget::PolyViewportWidget(QWidget* parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_NativeWindow);
	setMouseTracking(true);

	// TODO fix library names differences between platforms
	if (!LoadRenderingDevice.FunctionValid())
	{
		// Load rendering device library
		LoadRenderingDevice = Poly::LoadFunctionFromSharedLibrary<CreateRenderingDeviceFunc>("libRenderingDevice", "PolyCreateRenderingDevice");
		// Try to load library with different name
		if (!LoadRenderingDevice.FunctionValid())
			LoadRenderingDevice = Poly::LoadFunctionFromSharedLibrary<CreateRenderingDeviceFunc>("libpolyrenderingdevice", "PolyCreateRenderingDevice");
		ASSERTE(LoadRenderingDevice.FunctionValid(), "Library libRenderingDevice load failed");
		Poly::gConsole.LogDebug("Library libRenderingDevice loaded.");
	}
	
	if (!LoadGame.FunctionValid())
	{
		// Load game library
		LoadGame = Poly::LoadFunctionFromSharedLibrary<CreateGameFunc>("libGame", "CreateGame");
		// Try to load library with different name
		if (!LoadGame.FunctionValid())
			LoadGame = Poly::LoadFunctionFromSharedLibrary<CreateGameFunc>("libgame", "CreateGame");
		ASSERTE(LoadGame.FunctionValid(), "Library libGame load failed");
		Poly::gConsole.LogDebug("Library libGame loaded.");
	}
}

// ---------------------------------------------------------------------------------------------------------
void PolyViewportWidget::InitializeViewport()
{
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
void PolyViewportWidget::Update()
{	
	Poly::gEngine->Update();
}

// ---------------------------------------------------------------------------------------------------------
void PolyViewportWidget::resizeEvent(QResizeEvent* resizeEvent)
{
	Poly::ScreenSize screenSize;
	screenSize.Width = resizeEvent->size().width();
	screenSize.Height = resizeEvent->size().height();
	Poly::gEngine->ResizeScreen(screenSize);
}

// ---------------------------------------------------------------------------------------------------------
void PolyViewportWidget::wheelEvent(QWheelEvent* wheelEvent)
{
	Poly::gEngine->UpdateWheelPos(Poly::Vector2i(wheelEvent->delta(), 0));
}

// ---------------------------------------------------------------------------------------------------------
void PolyViewportWidget::mouseMoveEvent(QMouseEvent* mouseEvent)
{
	Poly::gEngine->UpdateMousePos(Poly::Vector2i(mouseEvent->pos().x(), mouseEvent->pos().y()));
}

// ---------------------------------------------------------------------------------------------------------
void PolyViewportWidget::mousePressEvent(QMouseEvent* mouseEvent)
{
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
void PolyViewportWidget::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
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
void PolyViewportWidget::keyPressEvent(QKeyEvent* keyEvent)
{
	Poly::gEngine->KeyDown(static_cast<Poly::eKey>(QtKeyEventToSDLScancode(keyEvent->nativeScanCode())));
}

// ---------------------------------------------------------------------------------------------------------
void PolyViewportWidget::keyReleaseEvent(QKeyEvent* keyEvent)
{
	if (keyEvent->isAutoRepeat())
		keyEvent->ignore();
	else
 		Poly::gEngine->KeyUp(static_cast<Poly::eKey>(QtKeyEventToSDLScancode(keyEvent->nativeScanCode())));
}