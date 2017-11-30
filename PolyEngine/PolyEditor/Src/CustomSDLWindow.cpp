#include "PolyEditorPCH.hpp"
#include "CustomSDLWindow.hpp"
#include <SDL.h>

CustomSDLWindow::CustomSDLWindow(SDL_Window* mainWin, SDL_Window* helperWin)
	: MainWindow(mainWin), HelperWindow(helperWin) {}

CustomSDLWindow::CustomSDLWindow(CustomSDLWindow&& rhs)
	: MainWindow(rhs.MainWindow), HelperWindow(rhs.HelperWindow)
{
	rhs.MainWindow = nullptr;
	rhs.HelperWindow = nullptr;
}

CustomSDLWindow& CustomSDLWindow::operator=(CustomSDLWindow&& rhs)
{
	if (MainWindow)
		SDL_DestroyWindow(MainWindow);
	if (HelperWindow)
		SDL_DestroyWindow(HelperWindow);

	MainWindow = rhs.MainWindow;
	HelperWindow = rhs.HelperWindow;
	rhs.MainWindow = nullptr;
	rhs.HelperWindow = nullptr;
	return *this;
}

CustomSDLWindow::~CustomSDLWindow()
{
	if (MainWindow)
		SDL_DestroyWindow(MainWindow);
	if (HelperWindow)
		SDL_DestroyWindow(HelperWindow);
}

CustomSDLWindow CustomSDLWindow::CreateSDLWindowFromArgs(void* nativeHandle, uint32_t flags)
{
	// Create temporary window to base flags from
	SDL_Window* tmp = SDL_CreateWindow(
		"PolyEngine standalone",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		1,
		1,
		flags | SDL_WINDOW_HIDDEN
	);

	// Window creation failed, return nullptr
	if (!tmp)
		return CustomSDLWindow{ nullptr, nullptr };

	// Create string from temporary window pointer and set proper window creation hint (!)
	char sBuf[32];
	sprintf(sBuf, "%p", tmp);
	SDL_SetHint(SDL_HINT_VIDEO_WINDOW_SHARE_PIXEL_FORMAT, sBuf);

	// Create result window handle from native handle
	return CustomSDLWindow(SDL_CreateWindowFrom(nativeHandle), tmp);
}

uint32_t QtKeyEventToSDLScancode(uint32_t nativeQtCode)
{
	switch (nativeQtCode)
	{
		//TODO fill this switch
	case 333: return SDL_SCANCODE_RIGHT;
	case 331: return SDL_SCANCODE_LEFT;
	case 328: return SDL_SCANCODE_UP;
	case 336: return SDL_SCANCODE_DOWN;
	case 57: return SDL_SCANCODE_SPACE;
	default:
		Poly::gConsole.LogDebug("Unhandled key: {}", nativeQtCode);
		return SDL_SCANCODE_UNKNOWN;
	}
}
