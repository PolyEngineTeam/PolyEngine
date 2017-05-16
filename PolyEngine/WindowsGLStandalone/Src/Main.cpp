// include the basic windows header file
#include <windows.h>
#include <windowsx.h>

#include <Engine.hpp>
#include <OpenGLRenderingContext.hpp>
#include <TestGame.hpp>
#include <sstream>

static Poly::Engine* gEngine = nullptr;

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam);

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	//Open console, good for now
	AllocConsole();
	FILE* s1, *s2, *s3;
	freopen_s(&s1, "CONIN$", "r", stdin);
	freopen_s(&s2, "CONOUT$", "w", stdout);
	freopen_s(&s3, "CONOUT$", "w", stderr);

	// the handle for the window, filled by a function
	HWND hWnd;
	// this struct holds information for the window class
	WNDCLASSEX wc;

	// clear out the window class for use
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	// fill in the struct with the needed information
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = "WindowClass1";

	// register the window class
	RegisterClassEx(&wc);

	RECT viewportRect = { 0, 0, 800, 600 };
	RECT windowRect = viewportRect;    // set the size, but not the position
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);    // adjust the size

																  // create the window and use the result as the handle
	hWnd = CreateWindowEx(NULL,
		"WindowClass1",    // name of the window class
		" Standalone - OpenGL",   // title of the window
		WS_OVERLAPPEDWINDOW,    // window style
		300,    // x-position of the window
		300,    // y-position of the window
		windowRect.right - windowRect.left,    // width of the window
		windowRect.bottom - windowRect.top,    // height of the window
		NULL,    // we have no parent window, NULL
		NULL,    // we aren't using menus, NULL
		hInstance,    // application handle
		NULL);    // used with multiple windows, NULL

				  // display the window on the screen
	ShowWindow(hWnd, nCmdShow);

	// enter the main loop:

	// this struct holds Windows event messages
	MSG msg;

	InvadersGame Game;
	Poly::OpenGLRenderingContextParams Context(hWnd, viewportRect);
	Poly::Engine Engine(&Game);
	gEngine = &Engine;
	bool result = Engine.Init(&Context);
	if (!result)
	{
		Poly::gConsole.LogError("Engine load failed!");
		exit(-1);
	}

	Poly::gConsole.LogDebug("Engine loaded successfully");

	// wait for the next message in the queue, store the result in 'msg'
	while (true)
	{
		// Check to see if any messages are waiting in the queue
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
		{
			// translate keystroke messages into the right format
			TranslateMessage(&msg);

			// send the message to the WindowProc function
			DispatchMessage(&msg);

		}
		
		// check to see if it's time to quit
		if (msg.message == WM_QUIT)
			break;

		// Run game code here
		Engine.Update();
    // Temporary visualisation of delta time in window title
    std::stringstream ss;
    ss << Poly::TimeSystem::GetTimerDeltaTime(&Engine.GetWorld(), Poly::eEngineTimer::SYSTEM);
    Poly::String windText("DeltaTime = ");
    windText = windText + ss.str().c_str();
    SetWindowText(hWnd, windText.GetCStr());
	}

	Engine.Deinit();

	// return this part of the WM_QUIT message to Windows
	return static_cast<int>(msg.wParam);
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// sort through and find what code to run for the message given
	switch (message)
	{

	case WM_SIZE:
	{
		Poly::ScreenSize screenSize;
		screenSize.Width = GET_X_LPARAM(lParam);
		screenSize.Height = GET_Y_LPARAM(lParam);
		if(gEngine)
			gEngine->ResizeScreen(screenSize);
		return 0;
	}

		// input
	// Use separate case's for mouse buttons because otherwise mouse buttons aren't checked
	case WM_LBUTTONDOWN:
		gEngine->KeyDown(Poly::eKey::MLBUTTON);
		return 0;
	case WM_LBUTTONUP:
		gEngine->KeyUp(Poly::eKey::MLBUTTON);
		return 0;

	case WM_RBUTTONDOWN:
		gEngine->KeyDown(Poly::eKey::MRBUTTON);
		return 0;
	case WM_RBUTTONUP:
		gEngine->KeyUp(Poly::eKey::MRBUTTON);
		return 0;

	case WM_MBUTTONDOWN:
		gEngine->KeyDown(Poly::eKey::MMBUTTON);
		return 0;
	case WM_MBUTTONUP:
		gEngine->KeyUp(Poly::eKey::MMBUTTON);
		return 0;

	case WM_XBUTTONDOWN:
		if(GET_Y_LPARAM(wParam) == XBUTTON1) gEngine->KeyDown(Poly::eKey::MBUTTON1);
		else gEngine->KeyDown(Poly::eKey::MBUTTON2);
		return 0;
	case WM_XBUTTONUP:
		if(GET_Y_LPARAM(wParam) == XBUTTON1) gEngine->KeyUp(Poly::eKey::MBUTTON1);
		else gEngine->KeyUp(Poly::eKey::MBUTTON2);
		return 0;

	case WM_KEYDOWN:
		gEngine->KeyDown(static_cast<Poly::eKey>((unsigned int)wParam));
		return 0;
	case WM_KEYUP:
		gEngine->KeyUp(static_cast<Poly::eKey>((unsigned int)wParam));
		return 0;

	case WM_MOUSEMOVE:
	{
		POINT pointPos;
		GetCursorPos(&pointPos);
		ScreenToClient(hWnd, &pointPos);
		gEngine->UpdateMousePos(Poly::Vector(static_cast<float>(pointPos.x), static_cast<float>(pointPos.y), 0));
		return 0;
	}
	case WM_MOUSEWHEEL:
	{
		int xPos = GET_WHEEL_DELTA_WPARAM(wParam);
		gEngine->UpdateWheelPos(Poly::Vector(static_cast<float>(xPos), 0, 0));
		return 0;
	}
	// end of input

		// this message is read when the window is closed
	case WM_DESTROY:
	{
		// close the application entirely
		PostQuitMessage(0);
		return 0;
	} break;
	}

	// Handle any messages the switch statement didn't
	return DefWindowProc(hWnd, message, wParam, lParam);
}
