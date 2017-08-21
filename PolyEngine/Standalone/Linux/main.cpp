#include <Engine.hpp>
#include <GLRenderingDevice.hpp>
#include <Game.hpp>

void handleEvents(Display* display, Window window, const XEvent& ev);

int main() {
	//open the display
	std::unique_ptr<Display, decltype(XCloseDisplay)*> display(XOpenDisplay(nullptr), &XCloseDisplay);
	if (!display) {
		Poly::gConsole.LogError("Could not open X11 display!");
		return 1;
	}
	Poly::gConsole.LogDebug("X11 display opened");

	int screen = DefaultScreen(display.get());

	//check GLX version
	{
		GLint majorGLX = 0, minorGLX = 0;
		glXQueryVersion(display.get(), &majorGLX, &minorGLX);
		if (majorGLX <= 1 && minorGLX < 4) {
			Poly::gConsole.LogError("GLX 1.4 or greater is required.");
			return 1;
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

	int fbcount;
	GLXFBConfig* fbc = glXChooseFBConfig(display.get(), screen, glxAttribs, &fbcount);
	if (!fbc) {
		Poly::gConsole.LogError("Failed to retrieve framebuffer config.");
		return 1;
	}
	GLXFBConfig fbConfig = *fbc;
	XFree(fbc);
	Poly::gConsole.LogDebug("Framebuffer config get");

	XVisualInfo* visual = glXGetVisualFromFBConfig(display.get(), fbConfig);
	Colormap colourMap;

	Poly::ScreenSize windowSize;
	windowSize.Width = 800;
	windowSize.Height = 600;

	//create the window
	XSetWindowAttributes windowAttribs;
	windowAttribs.colormap = colourMap = XCreateColormap(display.get(), RootWindow(display.get(), screen), visual->visual, AllocNone);
// 	windowAttribs.override_redirect = True; //ignore the window manager
	windowAttribs.event_mask        = ExposureMask | KeyPressMask | KeyReleaseMask /*| KeymapStateMask*/ | PointerMotionMask | ButtonPressMask | ButtonReleaseMask /*| EnterWindowMask | LeaveWindowMask | StructureNotifyMask*/;
	Window window = XCreateWindow(
		display.get(),
		RootWindow(display.get(), screen),                                     //parent
		0,                                                                     //x
		0,                                                                     //y
		windowSize.Width,                                                                   //width
		windowSize.Height,                                                                   //height
		0,                                                                     //border width
		visual->depth,
		InputOutput,                                                           //class
		visual->visual,
		/*CWBorderPixel |*/ CWColormap | CWEventMask /*| CWOverrideRedirect*/, //attributes that we are passing in windowAttribs
		&windowAttribs
	);
	XStoreName(display.get(), window, "Standalone - OpenGL");
	XFree(visual);

	if (window) {
		Poly::gConsole.LogDebug("X11 window created");
	} else {
		Poly::gConsole.LogError("Failed to create window.");
		return 1;
	}

	//allow window close event interception
	Atom atomWmDeleteWindow = XInternAtom(display.get(), "WM_DELETE_WINDOW", /*only_if_exists*/ False);
	XSetWMProtocols(display.get(), window, &atomWmDeleteWindow, /*count*/ 1);
	Poly::gConsole.LogDebug("WM_DELETE_WINDOW atom retrieved");

	auto windowCleanup = [display = display.get(), colourMap](Window* window) {
		XDestroyWindow(display, *window);
		XFreeColormap(display, colourMap);
	};
	std::unique_ptr<Window, decltype(windowCleanup)> windowCleanupGuard(&window, windowCleanup);

	Poly::Engine Engine;

	std::unique_ptr<Poly::IGame> game = std::unique_ptr<Poly::IGame>(new SGJGame());
	std::unique_ptr<Poly::IRenderingDevice> device = std::unique_ptr<Poly::IRenderingDevice>(PolyCreateRenderingDevice(display.get(), window, fbConfig, windowSize));

	Engine.Init(std::move(game), std::move(device));
	Poly::gConsole.LogDebug("Engine loaded");

	//show the window
	XClearWindow(display.get(), window);
	XMapRaised(display.get(), window);
	Poly::gConsole.LogDebug("X11 window shown");

	//enter the matri... *ekhm* game loop
	XEvent ev;
	for(;;)
	{
		if (XPending(display.get()) > 0)
		{
			XNextEvent(display.get(), &ev);

			handleEvents(display.get(), window, ev);
			if ((ev.type == ClientMessage && static_cast<Atom>(ev.xclient.data.l[0]) == atomWmDeleteWindow) || ev.type == DestroyNotify)
			{
				break;
			}
		} else {
			Engine.Update();
		}
	}
	Poly::gConsole.LogDebug("Game loop interrupted");

	Poly::gConsole.LogDebug("Engine suicide");
}

void handleEvents(Display* display, Window window, const XEvent& ev) {
// 	char str[25] = {0};
// 	KeySym keysym = 0;
// 	int len = 0;
	constexpr uint scrollUp = 4;
	constexpr uint scrollDown = 5;
	switch (ev.type) {
		case Expose: {
			XWindowAttributes attribs;
			XGetWindowAttributes(display, window, &attribs);
			Poly::gEngine->ResizeScreen(Poly::ScreenSize{attribs.width, attribs.height});
		}
		break;

// 		case KeymapNotify: {
// 			XRefreshKeyboardMapping(&ev.xmapping);
// 		}
// 		break;

		case KeyPress: {
// 			len = XLookupString(&ev.xkey, str, 25, &keysym, NULL);
			Poly::gEngine->KeyDown(static_cast<Poly::eKey>(ev.xkey.keycode));
		}
		break;

		case KeyRelease: Poly::gEngine->KeyUp(static_cast<Poly::eKey>(ev.xkey.keycode)); break; //TODO(vuko): should we ignore auto-repeat?
		case ButtonPress: {
			switch (ev.xbutton.button) {
				case scrollUp: Poly::gEngine->UpdateWheelPos(Poly::Vector(0.0f, -120.0f, 0.0f)); break;
				case scrollDown: Poly::gEngine->UpdateWheelPos(Poly::Vector(0.0f, 120.0f, 0.0f)); break;
				default: Poly::gEngine->KeyDown(static_cast<Poly::eKey>(ev.xbutton.button));
			}
		}
		break;

		case ButtonRelease: Poly::gEngine->KeyUp(static_cast<Poly::eKey>(ev.xbutton.button)); break;
		case MotionNotify: Poly::gEngine->UpdateMousePos(Poly::Vector(static_cast<float>(ev.xmotion.x), static_cast<float>(ev.xmotion.y), 0.0f)); break;
	}
}
