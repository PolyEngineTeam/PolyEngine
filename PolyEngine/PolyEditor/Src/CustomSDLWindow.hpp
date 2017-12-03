#pragma once
#include <QKeyEvent>

struct SDL_Window;

class CustomSDLWindow
{
public:
	static CustomSDLWindow CreateSDLWindowFromArgs(void* nativeHandle, uint32_t flags);

	CustomSDLWindow() = default;
	CustomSDLWindow(CustomSDLWindow&& rhs);
	CustomSDLWindow& operator=(CustomSDLWindow&& rhs);

	~CustomSDLWindow();

	inline bool IsValid() const { return MainWindow; }
	SDL_Window* Get() { return MainWindow; }
private:

	CustomSDLWindow(SDL_Window* mainWin, SDL_Window* helperWin);

	SDL_Window* MainWindow = nullptr;
	SDL_Window* HelperWindow = nullptr;
};

uint32_t QtKeyEventToSDLScancode(uint32_t key);