#include "PolyEditorPCH.hpp"
#include "GameplayViewportWidget.hpp"
#include <LibraryLoader.hpp>

#if defined(_WIN32)
	#include <windows.h>
	#include <windowsx.h>
#elif defined(__linux__) 
	//#include <epoxy/glx.h>
#elif defined(__APPLE__)
#else
#error "Unsupported platform :("
#endif

#include <QGLWidget>

extern "C"
{
#if defined(_WIN32)
	using CreateRenderingDeviceFunc = Poly::IRenderingDevice* (HWND hwnd, RECT rect);
#elif defined(__linux__) 
	using CreateRenderingDeviceFunc = Poly::IRenderingDevice* (Display* display, Window window, GLXFBConfig fbConfig, const Poly::ScreenSize& size);
#elif defined(__APPLE__)
	using CreateRenderingDeviceFunc = Poly::IRenderingDevice* (void* window, const Poly::ScreenSize& size);
#else
#error "Unsupported platform :("
#endif
	
	using CreateGameFunc = Poly::IGame* (void);
}

static Poly::LibraryFunctionHandle<CreateRenderingDeviceFunc> LoadRenderingDevice;
static Poly::LibraryFunctionHandle<CreateGameFunc> LoadGame;

// ---------------------------------------------------------------------------------------------------------
GameplayViewportWidget::GameplayViewportWidget(QWidget* parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_NativeWindow);
	setMouseTracking(true);

	// TODO fix library names differences between platforms
	if (!LoadRenderingDevice.FunctionValid())
	{
#if defined(_WIN32)
		LoadRenderingDevice = Poly::LoadFunctionFromSharedLibrary<CreateRenderingDeviceFunc>("libRenderingDevice", "PolyCreateRenderingDevice");
#elif defined(__linux__)  || defined(__APPLE__)
		LoadRenderingDevice = Poly::LoadFunctionFromSharedLibrary<CreateRenderingDeviceFunc>("libpolyrenderingdevice", "PolyCreateRenderingDevice");
#endif
		ASSERTE(LoadRenderingDevice.FunctionValid(), "Error loading rendering device DLL");
	}
	
	if (!LoadGame.FunctionValid())
	{
#if defined(_WIN32)
		LoadGame = Poly::LoadFunctionFromSharedLibrary<CreateGameFunc>("libGame", "CreateGame");
#elif defined(__linux__)  || defined(__APPLE__)
		LoadGame = Poly::LoadFunctionFromSharedLibrary<CreateGameFunc>("libgame", "CreateGame");
#endif
		ASSERTE(LoadGame.FunctionValid(), "Error loading rendering device DLL");
	}
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::InitializeViewport()
{
#if defined(_WIN32)
	RECT viewportRect;
	viewportRect.top = 0;
	viewportRect.left = 0;
	viewportRect.bottom = height();
	viewportRect.right = width();
#elif defined(__linux__)  || defined(__APPLE__)
	Poly::ScreenSize viewportRect;
	viewportRect.Width = width();
	viewportRect.Hidth = height();
#endif
	

	// TODO: catch winId changes (http://doc.qt.io/qt-5/qwidget.html#winId)
	// TODO: something like addviewport to rendering device
	std::unique_ptr<Poly::IGame> game = std::unique_ptr<Poly::IGame>(LoadGame());
	
#if defined(_WIN32)
	std::unique_ptr<Poly::IRenderingDevice> device = std::unique_ptr<Poly::IRenderingDevice>(LoadRenderingDevice((HWND)winId(), viewportRect));
#elif defined(__linux__)
	GLXFBConfig config = qglx_findConfig(QX11Info::display(), QX11Info::appScreen(), platformWindowFormat());
	std::unique_ptr<Poly::IRenderingDevice> device = std::unique_ptr<Poly::IRenderingDevice>(LoadRenderingDevice(QX11Info::display(), (Window)winId(), config, viewportRect));
#elif defined(__APPLE__)
	//TODO
	// use HIObjectRef(winId())
	std::unique_ptr<Poly::IRenderingDevice> device = std::unique_ptr<Poly::IRenderingDevice>(LoadRenderingDevice(winId(), viewportRect));
#endif

	Poly::gEngine->Init(std::move(game), std::move(device));
	Poly::gConsole.LogDebug("Engine loaded successfully");
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::Update()
{
	Poly::gEngine->Update();
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::resizeEvent(QResizeEvent* resizeEvent)
{
	Poly::ScreenSize screenSize;
	screenSize.Width = resizeEvent->size().width();
	screenSize.Height = resizeEvent->size().height();
	Poly::gEngine->ResizeScreen(screenSize);
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::wheelEvent(QWheelEvent* wheelEvent)
{
	Poly::gEngine->UpdateWheelPos(Poly::Vector(static_cast<float>(wheelEvent->delta()), 0, 0));
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::mouseMoveEvent(QMouseEvent* mouseEvent)
{
	Poly::gEngine->UpdateMousePos(Poly::Vector(static_cast<float>(mouseEvent->pos().x()), static_cast<float>(mouseEvent->pos().y()), 0));
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::mousePressEvent(QMouseEvent* mouseEvent)
{
	switch (mouseEvent->button())
	{
	case Qt::LeftButton:
		Poly::gEngine->KeyDown(static_cast<Poly::eKey>(Poly::eKey::MLBUTTON));
		break;

	case Qt::RightButton:
		Poly::gEngine->KeyDown(static_cast<Poly::eKey>(Poly::eKey::MRBUTTON));
		break;

	case Qt::MiddleButton:
		Poly::gEngine->KeyDown(static_cast<Poly::eKey>(Poly::eKey::MMBUTTON));
		break;

	default:
		// unsupported key
		break;
	}
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
	switch (mouseEvent->button())
	{
	case Qt::LeftButton:
		Poly::gEngine->KeyUp(static_cast<Poly::eKey>(Poly::eKey::MLBUTTON));
		break;

	case Qt::RightButton:
		Poly::gEngine->KeyUp(static_cast<Poly::eKey>(Poly::eKey::MRBUTTON));
		break;

	case Qt::MiddleButton:
		Poly::gEngine->KeyUp(static_cast<Poly::eKey>(Poly::eKey::MMBUTTON));
		break;

	default:
		// unsupported key
		break;
	}
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::keyPressEvent(QKeyEvent* keyEvent)
{
	Poly::gEngine->KeyDown(static_cast<Poly::eKey>((unsigned int)keyEvent->nativeVirtualKey()));
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::keyReleaseEvent(QKeyEvent* keyEvent)
{
	if (keyEvent->isAutoRepeat())
		keyEvent->ignore();
	else
 		Poly::gEngine->KeyUp(static_cast<Poly::eKey>((unsigned int)keyEvent->nativeVirtualKey()));
}