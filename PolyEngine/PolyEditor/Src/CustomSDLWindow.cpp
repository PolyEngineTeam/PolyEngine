#include "PolyEditorPCH.hpp"
#include "CustomSDLWindow.hpp"
#include <SDL.h>

#if defined(__APPLE__)
    #include "MacOSHelper.hpp"
#endif

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
	if (HelperWindow)
		SDL_DestroyWindow(HelperWindow);
	if (MainWindow)
		SDL_DestroyWindow(MainWindow);
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
#if defined(__APPLE__)
    SDL_Window* window = SDL_CreateWindowFrom(GetNSWindowFromNSView(nativeHandle));
#else
    SDL_Window* window = SDL_CreateWindowFrom(nativeHandle);
#endif
	return CustomSDLWindow(window, tmp);
}

uint32_t QtKeyEventToSDLScancode(uint32_t key)
{
	switch (key)
	{
		//TODO fill this switch
	case Qt::Key::Key_A: return SDL_SCANCODE_A;
	case Qt::Key::Key_B: return SDL_SCANCODE_B;
	case Qt::Key::Key_C: return SDL_SCANCODE_C;
	case Qt::Key::Key_D: return SDL_SCANCODE_D;
	case Qt::Key::Key_E: return SDL_SCANCODE_E;
	case Qt::Key::Key_F: return SDL_SCANCODE_F;
	case Qt::Key::Key_G: return SDL_SCANCODE_G;
	case Qt::Key::Key_H: return SDL_SCANCODE_H;
	case Qt::Key::Key_I: return SDL_SCANCODE_I;
	case Qt::Key::Key_J: return SDL_SCANCODE_J;
	case Qt::Key::Key_K: return SDL_SCANCODE_K;
	case Qt::Key::Key_L: return SDL_SCANCODE_L;
	case Qt::Key::Key_M: return SDL_SCANCODE_M;
	case Qt::Key::Key_N: return SDL_SCANCODE_N;
	case Qt::Key::Key_O: return SDL_SCANCODE_O;
	case Qt::Key::Key_P: return SDL_SCANCODE_P;
	case Qt::Key::Key_Q: return SDL_SCANCODE_Q;
	case Qt::Key::Key_R: return SDL_SCANCODE_R;
	case Qt::Key::Key_S: return SDL_SCANCODE_S;
	case Qt::Key::Key_T: return SDL_SCANCODE_T;
	case Qt::Key::Key_U: return SDL_SCANCODE_U;
	case Qt::Key::Key_V: return SDL_SCANCODE_V;
	case Qt::Key::Key_W: return SDL_SCANCODE_W;
	case Qt::Key::Key_X: return SDL_SCANCODE_X;
	case Qt::Key::Key_Y: return SDL_SCANCODE_Y;
	case Qt::Key::Key_Z: return SDL_SCANCODE_Z;

	case Qt::Key::Key_1: return SDL_SCANCODE_1;
	case Qt::Key::Key_2: return SDL_SCANCODE_2;
	case Qt::Key::Key_3: return SDL_SCANCODE_3;
	case Qt::Key::Key_4: return SDL_SCANCODE_4;
	case Qt::Key::Key_5: return SDL_SCANCODE_5;
	case Qt::Key::Key_6: return SDL_SCANCODE_6;
	case Qt::Key::Key_7: return SDL_SCANCODE_7;
	case Qt::Key::Key_8: return SDL_SCANCODE_8;
	case Qt::Key::Key_9: return SDL_SCANCODE_9;
	case Qt::Key::Key_0: return SDL_SCANCODE_0;

	case Qt::Key::Key_Return: return SDL_SCANCODE_RETURN;
	case Qt::Key::Key_Escape: return SDL_SCANCODE_ESCAPE;
	case Qt::Key::Key_Backspace: return SDL_SCANCODE_BACKSPACE;
	case Qt::Key::Key_Tab: return SDL_SCANCODE_TAB;
	case Qt::Key::Key_Space: return SDL_SCANCODE_SPACE;

	case Qt::Key::Key_Minus: return SDL_SCANCODE_MINUS;
	case Qt::Key::Key_Equal: return SDL_SCANCODE_EQUALS;
	case Qt::Key::Key_BracketLeft: return SDL_SCANCODE_LEFTBRACKET;
	case Qt::Key::Key_BracketRight: return SDL_SCANCODE_RIGHTBRACKET;
	case Qt::Key::Key_Backslash: return SDL_SCANCODE_BACKSLASH;
	case Qt::Key::Key_Semicolon: return SDL_SCANCODE_SEMICOLON;
	case Qt::Key::Key_Apostrophe: return SDL_SCANCODE_APOSTROPHE;
	case Qt::Key::Key_Agrave:
	case Qt::Key::Key_Dead_Grave:
	case Qt::Key::Key_Egrave:
	case Qt::Key::Key_Igrave:
	case Qt::Key::Key_Ograve:
	case Qt::Key::Key_Ugrave: return SDL_SCANCODE_GRAVE;
	case Qt::Key::Key_Comma: return SDL_SCANCODE_COMMA;
	case Qt::Key::Key_Period: return SDL_SCANCODE_PERIOD;
	case Qt::Key::Key_Slash: return SDL_SCANCODE_SLASH;

	case Qt::Key::Key_CapsLock: return SDL_SCANCODE_CAPSLOCK;

	case Qt::Key::Key_F1: return SDL_SCANCODE_F1;
	case Qt::Key::Key_F2: return SDL_SCANCODE_F2;
	case Qt::Key::Key_F3: return SDL_SCANCODE_F3;
	case Qt::Key::Key_F4: return SDL_SCANCODE_F4;
	case Qt::Key::Key_F5: return SDL_SCANCODE_F5;
	case Qt::Key::Key_F6: return SDL_SCANCODE_F6;
	case Qt::Key::Key_F7: return SDL_SCANCODE_F7;
	case Qt::Key::Key_F8: return SDL_SCANCODE_F8;
	case Qt::Key::Key_F9: return SDL_SCANCODE_F9;
	case Qt::Key::Key_F10: return SDL_SCANCODE_F10;
	case Qt::Key::Key_F11: return SDL_SCANCODE_F11;
	case Qt::Key::Key_F12: return SDL_SCANCODE_F12;
	case Qt::Key::Key_F13: return SDL_SCANCODE_F13;
	case Qt::Key::Key_F14: return SDL_SCANCODE_F14;
	case Qt::Key::Key_F15: return SDL_SCANCODE_F15;
	case Qt::Key::Key_F16: return SDL_SCANCODE_F16;
	case Qt::Key::Key_F17: return SDL_SCANCODE_F17;
	case Qt::Key::Key_F18: return SDL_SCANCODE_F18;
	case Qt::Key::Key_F19: return SDL_SCANCODE_F19;
	case Qt::Key::Key_F20: return SDL_SCANCODE_F20;
	case Qt::Key::Key_F21: return SDL_SCANCODE_F21;
	case Qt::Key::Key_F22: return SDL_SCANCODE_F22;
	case Qt::Key::Key_F23: return SDL_SCANCODE_F23;
	case Qt::Key::Key_F24: return SDL_SCANCODE_F24;

	case Qt::Key::Key_Print: return SDL_SCANCODE_PRINTSCREEN;
	case Qt::Key::Key_ScrollLock: return SDL_SCANCODE_SCROLLLOCK;
	case Qt::Key::Key_Pause: return SDL_SCANCODE_PAUSE;
	case Qt::Key::Key_Insert: return SDL_SCANCODE_INSERT;

	case Qt::Key::Key_Home: return SDL_SCANCODE_HOME;
	case Qt::Key::Key_PageUp: return SDL_SCANCODE_PAGEUP;
	case Qt::Key::Key_Delete: return SDL_SCANCODE_DELETE;
	case Qt::Key::Key_End: return SDL_SCANCODE_END;
	case Qt::Key::Key_PageDown: return SDL_SCANCODE_PAGEDOWN;
	case Qt::Key::Key_Right: return SDL_SCANCODE_RIGHT;
	case Qt::Key::Key_Left: return SDL_SCANCODE_LEFT;
	case Qt::Key::Key_Down: return SDL_SCANCODE_DOWN;
	case Qt::Key::Key_Up: return SDL_SCANCODE_UP;

	case Qt::Key::Key_NumLock:
	case Qt::Key::Key_Clear: return SDL_SCANCODE_NUMLOCKCLEAR;

	// TODO: fill numpad binding
	// case Qt::Key::Key return SDL_SCANCODE_KP_DIVIDE;
	// case Qt::Key::Key_F12: return SDL_SCANCODE_KP_MULTIPLY;
	// case Qt::Key::Key_F12: return SDL_SCANCODE_KP_MINUS;
	// case Qt::Key::Key_F12: return SDL_SCANCODE_KP_PLUS;
	// case Qt::Key::Key_F12: return SDL_SCANCODE_KP_ENTER;
	// case Qt::Key::Key_F12: return SDL_SCANCODE_KP_EQUALS;
	// case Qt::Key::Key_F12: return SDL_SCANCODE_KP_1;
	// case Qt::Key::Key_F12: return SDL_SCANCODE_KP_2;
	// case Qt::Key::Key_F12: return SDL_SCANCODE_KP_3;
	// case Qt::Key::Key_F12: return SDL_SCANCODE_KP_4;
	// case Qt::Key::Key_F12: return SDL_SCANCODE_KP_5;
	// case Qt::Key::Key_F12: return SDL_SCANCODE_KP_6;
	// case Qt::Key::Key_F12: return SDL_SCANCODE_KP_7;
	// case Qt::Key::Key_F12: return SDL_SCANCODE_KP_8;
	// case Qt::Key::Key_F12: return SDL_SCANCODE_KP_9;
	// case Qt::Key::Key_F12: return SDL_SCANCODE_KP_0;
	// case Qt::Key::Key_F12: return SDL_SCANCODE_KP_PERIOD;
	// case Qt::Key::Key_F12: return SDL_SCANCODE_KP_COMMA;

	case Qt::Key::Key_ApplicationLeft:
	case Qt::Key::Key_ApplicationRight: return SDL_SCANCODE_APPLICATION;

	case Qt::Key::Key_Execute: return SDL_SCANCODE_EXECUTE;
	case Qt::Key::Key_Help: return SDL_SCANCODE_HELP;
	case Qt::Key::Key_Menu: return SDL_SCANCODE_MENU;
	case Qt::Key::Key_Select: return SDL_SCANCODE_SELECT;
	case Qt::Key::Key_Stop: return SDL_SCANCODE_STOP;
	case Qt::Key::Key_Redo: return SDL_SCANCODE_AGAIN;
	case Qt::Key::Key_Undo: return SDL_SCANCODE_UNDO;
	case Qt::Key::Key_Cut: return SDL_SCANCODE_CUT;
	case Qt::Key::Key_Copy: return SDL_SCANCODE_COPY;
	case Qt::Key::Key_Paste: return SDL_SCANCODE_PASTE;
	case Qt::Key::Key_Find: return SDL_SCANCODE_FIND;
	case Qt::Key::Key_MicMute:
	case Qt::Key::Key_VolumeMute: return SDL_SCANCODE_MUTE;
	case Qt::Key::Key_MicVolumeDown:
	case Qt::Key::Key_VolumeUp: return SDL_SCANCODE_VOLUMEUP;
	case Qt::Key::Key_MicVolumeUp:
	case Qt::Key::Key_VolumeDown: return SDL_SCANCODE_VOLUMEDOWN;

	case Qt::Key::Key_Control: return SDL_SCANCODE_LCTRL;
	//case Qt::Key::Key_F12: return SDL_SCANCODE_RCTRL;
	case Qt::Key::Key_Shift: return SDL_SCANCODE_LSHIFT;
	//case Qt::Key::Key_F12: return SDL_SCANCODE_RSHIFT;
	case Qt::Key::Key_Alt: return SDL_SCANCODE_LALT;
	case Qt::Key::Key_AltGr: return SDL_SCANCODE_RALT;
	//case Qt::Key::win: return SDL_SCANCODE_LGUI;
	//case Qt::Key::win: return SDL_SCANCODE_RGUI;
	case Qt::Key::Key_Mode_switch: return SDL_SCANCODE_MODE;

	default:
		Poly::gConsole.LogDebug("Unhandled key: {}", key);
		return SDL_SCANCODE_UNKNOWN;
	}
}
