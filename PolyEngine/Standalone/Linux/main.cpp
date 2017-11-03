#include <Engine.hpp>
#include <epoxy/glx.h>
#include <dlfcn.h>

extern "C" {
	//using CreateRenderingDeviceFunc = decltype(PolyCreateRenderingDevice);
	//using LoadGameFunc = decltype(CreateGame);
	using CreateRenderingDeviceFunc = Poly::IRenderingDevice* (Display* display, Window window, GLXFBConfig fbConfig, const Poly::ScreenSize& size);
	using LoadGameFunc = Poly::IGame* ();
}

void handleEvents(Display* display, Window window, const XEvent& ev);

struct RawFunc {
	void* libraryHandle;
	void* func;
};
template<typename Function>
class DynLoaded {
public:
	//note(vuko): thankfully casting void* to func-ptr is defined and allowed in C++11 and up ._. UBSan still complains though :(
	DynLoaded(RawFunc raw) : handle(raw.libraryHandle), func(reinterpret_cast<Function*>(raw.func)) {}
	DynLoaded(DynLoaded&& other) : DynLoaded{RawFunc{other.handle, reinterpret_cast<void*>(other.func)}} { other.handle = nullptr; other.func = nullptr; }
	~DynLoaded() { if (handle) { dlclose(handle); } }
	template<typename... Args> inline typename std::result_of<Function*(Args...)>::type operator()(Args... args) { return func(std::forward<Args>(args)...); }
	inline bool FunctionValid() { return static_cast<bool>(func); }
private:
	void* handle;
	Function* func;
};
inline DynLoaded<CreateRenderingDeviceFunc> GetRenderingDeviceCreator(const char* soname);
inline DynLoaded<LoadGameFunc> GetGameLoader(const char* soname);


int main() { //todo(vuko): command-line args
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

	Poly::ScreenSize windowSize{800, 600};

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
		windowSize.Width,                                                      //width
		windowSize.Height,                                                     //height
		0,                                                                     //border width
		visual->depth,
		InputOutput,                                                           //class
		visual->visual,
		/*CWBorderPixel |*/ CWColormap | CWEventMask /*| CWOverrideRedirect*/, //attributes that we are passing in windowAttribs
		&windowAttribs
	);
	XStoreName(display.get(), window, "PolyEngine - OpenGL");
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

	std::unique_ptr<Poly::IRenderingDevice> renderingDevice;
	auto createRenderingDevice = GetRenderingDeviceCreator("libpolyrenderingdevice.so");
	if (createRenderingDevice.FunctionValid()) {
		auto newDevice = createRenderingDevice(display.get(), window, fbConfig, windowSize);
		renderingDevice.reset(newDevice);
	} else {
		return 1; //error message already handled
	}
	Poly::gConsole.LogDebug("Rendering device loaded");

	std::unique_ptr<Poly::IGame> game;
	auto loadGame = GetGameLoader("libgame.so"); //the name could be passed as a command-line arg
	if (loadGame.FunctionValid()) {
		auto newGame = loadGame();
		game.reset(newGame);
	} else {
		return 1; //error message already handled
	}
	Poly::gConsole.LogDebug("Game loaded");

	Poly::Engine Engine;
	Engine.Init(std::move(game), std::move(renderingDevice));
	Poly::gConsole.LogDebug("Engine loaded");

	//show the window
	XClearWindow(display.get(), window);
	XMapRaised(display.get(), window);
	Poly::gConsole.LogDebug("X11 window shown");

	//enter the matri... *ekhm* game loop
	XEvent ev;
	for(;;) {
		if (XPending(display.get()) > 0) {
			XNextEvent(display.get(), &ev);

			handleEvents(display.get(), window, ev);
			if ((ev.type == ClientMessage && static_cast<Atom>(ev.xclient.data.l[0]) == atomWmDeleteWindow) || ev.type == DestroyNotify) {
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

RawFunc LoadFunction(const char* library, const char* functionName) {
	void* handle = dlopen(library, RTLD_NOW /*| RTLD_GLOBAL*/); //don't be lazy in resolving symbols /*and allow subsequently loaded libs to use them*/
	if (const char* err = dlerror()) { //we could simply check if the handle is null, but using dlerror() doubles as clearing error flags
		Poly::gConsole.LogError("{}", err);
		Poly::gConsole.LogError("Use a game run-script or set the LD_LIBRARY_PATH environment variable manually");
		return {nullptr, nullptr};
	}

	void* func = dlsym(handle, functionName);
	if(const char* err = dlerror()) { //symbols can be legally null, so we need to check for errors instead
		Poly::gConsole.LogError("{}", err);
		dlclose(handle);
		return {nullptr, nullptr};
	}

	return {handle, func};
}

inline DynLoaded<CreateRenderingDeviceFunc> GetRenderingDeviceCreator(const char* soname) {
	return {LoadFunction(soname, "PolyCreateRenderingDevice")};
}

inline DynLoaded<LoadGameFunc> GetGameLoader(const char* soname) {
	return {LoadFunction(soname, "CreateGame")};
}
