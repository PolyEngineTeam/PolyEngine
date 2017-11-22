#include "PolyEditorPCH.hpp"
#include "GameplayViewportWidget.hpp"

#include <windows.h>
#include <windowsx.h>

// ---------------------------------------------------------------------------------------------------------
GameplayViewportWidget::GameplayViewportWidget(QWidget* parent)
	: QOpenGLWidget(parent)
{
	setAttribute(Qt::WA_NativeWindow, true);
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::LoadEditor()
{
	if (Engine != nullptr)
	{
		Engine->RequestGameQuit();
		Engine->Update();
		delete Engine;
	}

	Engine = new Poly::Engine();

	RECT viewportRect;
	viewportRect.top = 0;
	viewportRect.left = 0;
	viewportRect.right = width();
	viewportRect.bottom = height();

	std::unique_ptr<Poly::IGame> game = std::unique_ptr<Poly::IGame>(LoadGameDll(""));
	std::unique_ptr<Poly::IRenderingDevice> device = std::unique_ptr<Poly::IRenderingDevice>(LoadRenderingDeviceDll((HWND)effectiveWinId(), viewportRect, ""));

	Engine->Init(std::move(game), std::move(device));
	Poly::gConsole.LogDebug("Engine loaded successfully");
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::LoadGame(Poly::String path)
{
	if (Engine != nullptr)
	{
		Engine->RequestGameQuit();
		Engine->Update();
		delete Engine;
	}

	Engine = new Poly::Engine();

	RECT viewportRect;
	viewportRect.top = 0;
	viewportRect.left = 0;
	viewportRect.right = width();
	viewportRect.bottom = height();

	std::unique_ptr<Poly::IGame> game = std::unique_ptr<Poly::IGame>(LoadGameDll("gemepath"));
	std::unique_ptr<Poly::IRenderingDevice> device = std::unique_ptr<Poly::IRenderingDevice>(LoadRenderingDeviceDll((HWND)effectiveWinId(), viewportRect, "gemepath"));

	Engine->Init(std::move(game), std::move(device));
	Poly::gConsole.LogDebug("Engine loaded successfully");
}

void GameplayViewportWidget::Update()
{
	Engine->Update();
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::initializeGL()
{
	LoadEditor();
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::paintGL()
{
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::resizeGL(int w, int h)
{
	Poly::ScreenSize screenSize;
	screenSize.Width = width();
	screenSize.Height = height();
	Poly::gEngine->ResizeScreen(screenSize);
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

// ---------------------------------------------------------------------------------------------------------
Poly::IRenderingDevice* GameplayViewportWidget::LoadRenderingDeviceDll(HWND hwnd, RECT rect, Poly::String path)
{
	typedef Poly::IRenderingDevice* (__stdcall *RenderingDeviceGetter_t)(HWND, RECT);

	HINSTANCE hGetProcIDDLL = LoadLibrary(L"libRenderingDevice.dll");
	ASSERTE(hGetProcIDDLL, "could not load the dynamic library");

	RenderingDeviceGetter_t getRenderingDevice = (RenderingDeviceGetter_t)GetProcAddress(hGetProcIDDLL, "PolyCreateRenderingDevice");
	ASSERTE(getRenderingDevice, "could not locate the function");

	return getRenderingDevice(hwnd, rect);
}

// ---------------------------------------------------------------------------------------------------------
Poly::IGame* GameplayViewportWidget::LoadGameDll(Poly::String path)
{
	typedef Poly::IGame* (__stdcall *GameGetter_t)();

	HINSTANCE hGetProcIDDLL = LoadLibrary(L"libGame.dll");
	ASSERTE(hGetProcIDDLL, "could not load the dynamic library");

	GameGetter_t getGame = (GameGetter_t)GetProcAddress(hGetProcIDDLL, "CreateGame");
	ASSERTE(getGame, "could not locate the function");

	return getGame();
}