#include <PolyEditorPCH.hpp>
#include <CustomSDLWindow.hpp>
#include <Utils/Logger.hpp>
#include <SDL.h>

#if defined(__APPLE__)
    #include <MacOSHelper.hpp>
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

SDL_Keycode QtKeyEventToSDLKeycode(Qt::Key key)
{
	switch (key)
	{
		//TODO fill this switch
	case Qt::Key::Key_A: return SDLK_a;
	case Qt::Key::Key_B: return SDLK_b;
	case Qt::Key::Key_C: return SDLK_c;
	case Qt::Key::Key_D: return SDLK_d;
	case Qt::Key::Key_E: return SDLK_e;
	case Qt::Key::Key_F: return SDLK_f;
	case Qt::Key::Key_G: return SDLK_g;
	case Qt::Key::Key_H: return SDLK_h;
	case Qt::Key::Key_I: return SDLK_i;
	case Qt::Key::Key_J: return SDLK_j;
	case Qt::Key::Key_K: return SDLK_k;
	case Qt::Key::Key_L: return SDLK_l;
	case Qt::Key::Key_M: return SDLK_m;
	case Qt::Key::Key_N: return SDLK_n;
	case Qt::Key::Key_O: return SDLK_o;
	case Qt::Key::Key_P: return SDLK_p;
	case Qt::Key::Key_Q: return SDLK_q;
	case Qt::Key::Key_R: return SDLK_r;
	case Qt::Key::Key_S: return SDLK_s;
	case Qt::Key::Key_T: return SDLK_t;
	case Qt::Key::Key_U: return SDLK_u;
	case Qt::Key::Key_V: return SDLK_v;
	case Qt::Key::Key_W: return SDLK_w;
	case Qt::Key::Key_X: return SDLK_x;
	case Qt::Key::Key_Y: return SDLK_y;
	case Qt::Key::Key_Z: return SDLK_z;

	case Qt::Key::Key_1: return SDLK_1;
	case Qt::Key::Key_2: return SDLK_2;
	case Qt::Key::Key_3: return SDLK_3;
	case Qt::Key::Key_4: return SDLK_4;
	case Qt::Key::Key_5: return SDLK_5;
	case Qt::Key::Key_6: return SDLK_6;
	case Qt::Key::Key_7: return SDLK_7;
	case Qt::Key::Key_8: return SDLK_8;
	case Qt::Key::Key_9: return SDLK_9;
	case Qt::Key::Key_0: return SDLK_0;

	case Qt::Key::Key_Return: return SDLK_RETURN;
	case Qt::Key::Key_Escape: return SDLK_ESCAPE;
	case Qt::Key::Key_Backspace: return SDLK_BACKSPACE;
	case Qt::Key::Key_Tab: return SDLK_TAB;
	case Qt::Key::Key_Space: return SDLK_SPACE;

	case Qt::Key::Key_Minus: return SDLK_MINUS;
	case Qt::Key::Key_Equal: return SDLK_EQUALS;
	case Qt::Key::Key_BracketLeft: return SDLK_LEFTBRACKET;
	case Qt::Key::Key_BracketRight: return SDLK_RIGHTBRACKET;
	case Qt::Key::Key_Backslash: return SDLK_BACKSLASH;
	case Qt::Key::Key_Semicolon: return SDLK_SEMICOLON;
	case Qt::Key::Key_Apostrophe: return SDLK_QUOTE;
	case Qt::Key::Key_Agrave:
	case Qt::Key::Key_Dead_Grave:
	case Qt::Key::Key_Egrave:
	case Qt::Key::Key_Igrave:
	case Qt::Key::Key_Ograve:
	case Qt::Key::Key_Ugrave: return SDLK_BACKQUOTE;
	case Qt::Key::Key_Comma: return SDLK_COMMA;
	case Qt::Key::Key_Period: return SDLK_PERIOD;
	case Qt::Key::Key_Slash: return SDLK_SLASH;

	case Qt::Key::Key_CapsLock: return SDLK_CAPSLOCK;

	case Qt::Key::Key_F1: return SDLK_F1;
	case Qt::Key::Key_F2: return SDLK_F2;
	case Qt::Key::Key_F3: return SDLK_F3;
	case Qt::Key::Key_F4: return SDLK_F4;
	case Qt::Key::Key_F5: return SDLK_F5;
	case Qt::Key::Key_F6: return SDLK_F6;
	case Qt::Key::Key_F7: return SDLK_F7;
	case Qt::Key::Key_F8: return SDLK_F8;
	case Qt::Key::Key_F9: return SDLK_F9;
	case Qt::Key::Key_F10: return SDLK_F10;
	case Qt::Key::Key_F11: return SDLK_F11;
	case Qt::Key::Key_F12: return SDLK_F12;
	case Qt::Key::Key_F13: return SDLK_F13;
	case Qt::Key::Key_F14: return SDLK_F14;
	case Qt::Key::Key_F15: return SDLK_F15;
	case Qt::Key::Key_F16: return SDLK_F16;
	case Qt::Key::Key_F17: return SDLK_F17;
	case Qt::Key::Key_F18: return SDLK_F18;
	case Qt::Key::Key_F19: return SDLK_F19;
	case Qt::Key::Key_F20: return SDLK_F20;
	case Qt::Key::Key_F21: return SDLK_F21;
	case Qt::Key::Key_F22: return SDLK_F22;
	case Qt::Key::Key_F23: return SDLK_F23;
	case Qt::Key::Key_F24: return SDLK_F24;

	case Qt::Key::Key_Print: return SDLK_PRINTSCREEN;
	case Qt::Key::Key_ScrollLock: return SDLK_SCROLLLOCK;
	case Qt::Key::Key_Pause: return SDLK_PAUSE;
	case Qt::Key::Key_Insert: return SDLK_INSERT;

	case Qt::Key::Key_Home: return SDLK_HOME;
	case Qt::Key::Key_PageUp: return SDLK_PAGEUP;
	case Qt::Key::Key_Delete: return SDLK_DELETE;
	case Qt::Key::Key_End: return SDLK_END;
	case Qt::Key::Key_PageDown: return SDLK_PAGEDOWN;
	case Qt::Key::Key_Right: return SDLK_RIGHT;
	case Qt::Key::Key_Left: return SDLK_LEFT;
	case Qt::Key::Key_Down: return SDLK_DOWN;
	case Qt::Key::Key_Up: return SDLK_UP;

	case Qt::Key::Key_NumLock:
	case Qt::Key::Key_Clear: return SDLK_NUMLOCKCLEAR;

	// TODO: fill numpad binding
	// case Qt::Key::Key return SDLK_KP_DIVIDE;
	// case Qt::Key::Key_F12: return SDLK_KP_MULTIPLY;
	// case Qt::Key::Key_F12: return SDLK_KP_MINUS;
	// case Qt::Key::Key_F12: return SDLK_KP_PLUS;
	// case Qt::Key::Key_F12: return SDLK_KP_ENTER;
	// case Qt::Key::Key_F12: return SDLK_KP_EQUALS;
	// case Qt::Key::Key_F12: return SDLK_KP_1;
	// case Qt::Key::Key_F12: return SDLK_KP_2;
	// case Qt::Key::Key_F12: return SDLK_KP_3;
	// case Qt::Key::Key_F12: return SDLK_KP_4;
	// case Qt::Key::Key_F12: return SDLK_KP_5;
	// case Qt::Key::Key_F12: return SDLK_KP_6;
	// case Qt::Key::Key_F12: return SDLK_KP_7;
	// case Qt::Key::Key_F12: return SDLK_KP_8;
	// case Qt::Key::Key_F12: return SDLK_KP_9;
	// case Qt::Key::Key_F12: return SDLK_KP_0;
	// case Qt::Key::Key_F12: return SDLK_KP_PERIOD;
	// case Qt::Key::Key_F12: return SDLK_KP_COMMA;

	case Qt::Key::Key_ApplicationLeft:
	case Qt::Key::Key_ApplicationRight: return SDLK_APPLICATION;

	case Qt::Key::Key_Execute: return SDLK_EXECUTE;
	case Qt::Key::Key_Help: return SDLK_HELP;
	case Qt::Key::Key_Menu: return SDLK_MENU;
	case Qt::Key::Key_Select: return SDLK_SELECT;
	case Qt::Key::Key_Stop: return SDLK_STOP;
	case Qt::Key::Key_Redo: return SDLK_AGAIN;
	case Qt::Key::Key_Undo: return SDLK_UNDO;
	case Qt::Key::Key_Cut: return SDLK_CUT;
	case Qt::Key::Key_Copy: return SDLK_COPY;
	case Qt::Key::Key_Paste: return SDLK_PASTE;
	case Qt::Key::Key_Find: return SDLK_FIND;
	case Qt::Key::Key_MicMute:
	case Qt::Key::Key_VolumeMute: return SDLK_MUTE;
	case Qt::Key::Key_MicVolumeDown:
	case Qt::Key::Key_VolumeUp: return SDLK_VOLUMEUP;
	case Qt::Key::Key_MicVolumeUp:
	case Qt::Key::Key_VolumeDown: return SDLK_VOLUMEDOWN;

	// TODO left/right control/shift/alt handling
	case Qt::Key::Key_Control: return SDLK_LCTRL;
	//case Qt::Key::Key_F12: return SDLK_RCTRL;
	case Qt::Key::Key_Shift: return SDLK_LSHIFT;
	//case Qt::Key::Key_F12: return SDLK_RSHIFT;
	case Qt::Key::Key_Alt: return SDLK_LALT;
	case Qt::Key::Key_AltGr: return SDLK_RALT;
	//case Qt::Key::win: return SDLK_LGUI;
	//case Qt::Key::win: return SDLK_RGUI;
	case Qt::Key::Key_Mode_switch: return SDLK_MODE;

	default:
		Poly::gConsole.LogDebug("Unhandled key: {}", key);
		return SDLK_UNKNOWN;
	}
}
