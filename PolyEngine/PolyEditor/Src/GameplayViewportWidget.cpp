#include "PolyEditorPCH.hpp"
#include "GameplayViewportWidget.hpp"
#include <LibraryLoader.hpp>

#include <windows.h>
#include <windowsx.h>

using CreateRenderingDeviceInEditorFunc = Poly::IRenderingDevice* (const Poly::ScreenSize& size);
using CreateGameFunc = Poly::IGame* (void);

static Poly::LibraryFunctionHandle<CreateRenderingDeviceInEditorFunc> LoadRenderingDeviceInEditor;
static Poly::LibraryFunctionHandle<CreateGameFunc> LoadGame;

// ---------------------------------------------------------------------------------------------------------
GameplayViewportWidget::GameplayViewportWidget(QWidget* parent)
	: QOpenGLWidget(parent)
{
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::LoadEditor()
{
	ASSERTE(!Engine, "Engine was already created!");
	Engine = std::make_unique<Poly::Engine>();

	Poly::ScreenSize viewportRect;
	viewportRect.Width = width();
	viewportRect.Height = height();

	std::unique_ptr<Poly::IGame> game = std::unique_ptr<Poly::IGame>(LoadGame());
	std::unique_ptr<Poly::IRenderingDevice> device = std::unique_ptr<Poly::IRenderingDevice>(LoadRenderingDeviceInEditor(viewportRect));

	Engine->Init(std::move(game), std::move(device));
	Poly::gConsole.LogDebug("Engine loaded successfully");
}

void GameplayViewportWidget::Update()
{
	
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::initializeGL()
{
	LoadRenderingDeviceInEditor = Poly::LoadFunctionFromSharedLibrary<CreateRenderingDeviceInEditorFunc>("libRenderingDevice", "PolyCreateRenderingDeviceInEditor");
	ASSERTE(LoadRenderingDeviceInEditor.FunctionValid(), "Error loading rendering device DLL");
	LoadGame = Poly::LoadFunctionFromSharedLibrary<CreateGameFunc>("libGame", "CreateGame");
	ASSERTE(LoadGame.FunctionValid(), "Error loading rendering device DLL");

	LoadEditor();
}

// ---------------------------------------------------------------------------------------------------------
void GameplayViewportWidget::paintGL()
{
	Poly::gEngine->Update();
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