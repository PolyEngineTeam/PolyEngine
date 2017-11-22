#include "CppInterface.hpp"

#include <memory>

#include <Core.hpp>
#include <Engine.hpp>
#include <LibraryLoader.hpp>

using namespace Poly;

#include <sstream>

extern "C" {
    using CreateRenderingDeviceFunc = Poly::IRenderingDevice* (void* window, const Poly::ScreenSize& size);
    using LoadGameFunc = Poly::IGame* ();
}


//-----------------------------------------------------------------------------------------------
static std::unique_ptr<Engine> engine;
static LibraryFunctionHandle<CreateRenderingDeviceFunc> loadRenderingDevice;
static LibraryFunctionHandle<LoadGameFunc> loadGame;

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
// API exported to obj-C
void PolyEngineLoad(void* window, unsigned width, unsigned height)
{
    gConsole.LogError("Init engine load");

    ScreenSize size;
    size.Width = width;
    size.Height = height;

    engine = std::make_unique<Engine>();

    // Load game dylib
    std::unique_ptr<IGame> game;
    loadGame = LoadFunctionFromSharedLibrary<LoadGameFunc>("libgame", "CreateGame"); //the name could be passed as a command-line arg
    if (loadGame.FunctionValid())
        game.reset(loadGame());
    ASSERTE(game, "Game load failed!");

    // Load rendering device dylib
    std::unique_ptr<IRenderingDevice> device;
    loadRenderingDevice = LoadFunctionFromSharedLibrary<CreateRenderingDeviceFunc>("libpolyrenderingdevice", "PolyCreateRenderingDevice");
    if (loadRenderingDevice.FunctionValid())
         device.reset(loadRenderingDevice(window, size));
    ASSERTE(device, "Rendering device load failed!");

    engine->Init(std::move(game), std::move(device));
    gConsole.LogError("Engine loaded");
}

void PolyEngineUpdate()
{
    engine->Update();
}

void PolyEngineRelease()
{
    engine.reset();
}

void PolyEngineResize(unsigned width, unsigned height)
{
    gConsole.LogError("Window resize {} {}", width, height);
    ScreenSize size;
    size.Width = width;
    size.Height = height;
    engine->ResizeScreen(size);
}

void PolyEngineKeyDown(unsigned keyCode)
{
    engine->KeyDown(static_cast<eKey>(keyCode));
}

void PolyEngineKeyUp(unsigned keyCode)
{
    engine->KeyUp(static_cast<eKey>(keyCode));
}

void PolyEngineUpdateMousePos(unsigned x, unsigned y)
{
    engine->UpdateMousePos(Vector(static_cast<float>(x), static_cast<float>(y), 0.f));
}

void PolyEngineUpdateWheelPos(unsigned x, unsigned y)
{
    engine->UpdateWheelPos(Vector(static_cast<float>(x), static_cast<float>(y), 0.f));
}
