#include "PolyEditorPCH.hpp"
#include <qtextstream.h>
#include "PolyViewportWidget.hpp"
#include <Engine.hpp>
#include <LibraryLoader.hpp>
#include <QtGui/QtGui>


#if defined(_WIN32)
	#include <windows.h>
	#include <windowsx.h>
#elif defined(__linux__) 
	#include <QX11Info>
    #include <X11/Xlib.h>
    #include <GL/glx.h>
#elif defined(__APPLE__)
#else
#error "Unsupported platform :("
#endif

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
PolyViewportWidget::PolyViewportWidget(QWidget* parent)
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
void PolyViewportWidget::InitializeViewport()
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
	viewportRect.Height = height();
#endif
	

	// TODO: catch winId changes (http://doc.qt.io/qt-5/qwidget.html#winId)
	// TODO: something like addviewport to rendering device
	std::unique_ptr<Poly::IGame> game = std::unique_ptr<Poly::IGame>(LoadGame());
#if defined(_WIN32)
	std::unique_ptr<Poly::IRenderingDevice> device = std::unique_ptr<Poly::IRenderingDevice>(LoadRenderingDevice((HWND)winId(), viewportRect));
#elif defined(__linux__)
	//GLXFBConfig config = qglx_findConfig(QX11Info::display(), QX11Info::appScreen(), this->platformWindowFormat());

    //check GLX version
    {
        GLint majorGLX = 0, minorGLX = 0;
        glXQueryVersion(QX11Info::display(), &majorGLX, &minorGLX);
        if (majorGLX <= 1 && minorGLX < 4) {
            Poly::gConsole.LogError("GLX 1.4 or greater is required.");
            std::exit(1);
        }
    }

    GLint glxAttribs[] = {
            GLX_X_RENDERABLE  , True,
            GLX_DRAWABLE_TYPE , GLX_WINDOW_BIT,
            GLX_RENDER_TYPE   , GLX_RGBA_BIT,
            GLX_X_VISUAL_TYPE , GLX_TRUE_COLOR,
            GLX_RED_SIZE      , 8,
            GLX_GREEN_SIZE    , 8,
            GLX_BLUE_SIZE     , 8,
            GLX_ALPHA_SIZE    , 8,
            GLX_DEPTH_SIZE    , 24,
            GLX_STENCIL_SIZE  , 8,
            GLX_DOUBLEBUFFER  , True,
            GLX_SAMPLE_BUFFERS, False, //MSAA off
            GLX_SAMPLES       , 0,
            None
    };

    int fbcount = 0;
    // This part is crashing for now.
    GLXFBConfig* fbc = glXChooseFBConfig(QX11Info::display(), QX11Info::appScreen(), glxAttribs, &fbcount);
    if (!fbc) {
        Poly::gConsole.LogError("Failed to retrieve framebuffer config.");
        std::exit(1);
    }
    GLXFBConfig fbConfig = *fbc;
    XFree(fbc);
    Poly::gConsole.LogDebug("Framebuffer config get");

    ASSERTE(fbcount > 0, "Invalid fb count");
	std::unique_ptr<Poly::IRenderingDevice> device = std::unique_ptr<Poly::IRenderingDevice>(LoadRenderingDevice(QX11Info::display(), (Window)winId(), fbConfig, viewportRect));
#elif defined(__APPLE__)
	//TODO
	// use HIObjectRef(winId())
	std::unique_ptr<Poly::IRenderingDevice> device = std::unique_ptr<Poly::IRenderingDevice>(LoadRenderingDevice(winId(), viewportRect));
#endif
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
	Poly::gEngine->UpdateWheelPos(Poly::Vector(static_cast<float>(wheelEvent->delta()), 0, 0));
}

// ---------------------------------------------------------------------------------------------------------
void PolyViewportWidget::mouseMoveEvent(QMouseEvent* mouseEvent)
{
	Poly::gEngine->UpdateMousePos(Poly::Vector(static_cast<float>(mouseEvent->pos().x()), static_cast<float>(mouseEvent->pos().y()), 0));
}

// ---------------------------------------------------------------------------------------------------------
void PolyViewportWidget::mousePressEvent(QMouseEvent* mouseEvent)
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
void PolyViewportWidget::mouseReleaseEvent(QMouseEvent* mouseEvent)
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
void PolyViewportWidget::keyPressEvent(QKeyEvent* keyEvent)
{
	Poly::gEngine->KeyDown(static_cast<Poly::eKey>((unsigned int)keyEvent->nativeVirtualKey()));
}

// ---------------------------------------------------------------------------------------------------------
void PolyViewportWidget::keyReleaseEvent(QKeyEvent* keyEvent)
{
	if (keyEvent->isAutoRepeat())
		keyEvent->ignore();
	else
 		Poly::gEngine->KeyUp(static_cast<Poly::eKey>((unsigned int)keyEvent->nativeVirtualKey()));
}