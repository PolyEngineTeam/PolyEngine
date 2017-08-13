#include "PolyEditorPCH.hpp"
#include "EngineLoaderHelper.hpp"

#if defined(_WIN32)
#include <windows.h>
#include <windowsx.h>


Poly::IRenderingDevice* LoadRenderingDevice()
{
	typedef Poly::IRenderingDevice* (__stdcall *RenderingDeviceGetter_t)(HWND, RECT);

	/*HINSTANCE hGetProcIDDLL = LoadLibrary("libRenderingDevice.dll");
	ASSERTE(hGetProcIDDLL, "could not load the dynamic library");

	RenderingDeviceGetter_t getRenderingDevice = (RenderingDeviceGetter_t)GetProcAddress(hGetProcIDDLL, "PolyCreateRenderingDevice");
	ASSERTE(getRenderingDevice, "could not locate the function");

	return getRenderingDevice(hwnd, rect);*/
	return nullptr;
}

Poly::IGame* LoadGame()
{
	typedef Poly::IGame* (__stdcall *GameGetter_t)();

	HINSTANCE hGetProcIDDLL = LoadLibrary((LPCWSTR)"libGame.dll");
	ASSERTE(hGetProcIDDLL, "could not load the dynamic library");

	GameGetter_t getGame = (GameGetter_t)GetProcAddress(hGetProcIDDLL, "CreateGame");
	ASSERTE(getGame, "could not locate the function");

	return getGame();
}

#endif

