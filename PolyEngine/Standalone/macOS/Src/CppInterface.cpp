#include "CppInterface.hpp"

#include <memory>

#include <Core.hpp>
#include <Engine.hpp>
#include <GLRenderingDevice.hpp>
#include <Game.hpp>

using namespace Poly;

static std::unique_ptr<Engine> engine;

#include <sstream>

IGame* LoadGame()
{
    // TODO dynamic dylib loading
    return new SGJGame();
}

IRenderingDevice* LoadRenderingDevice(void* window, unsigned width, unsigned height)
{
    // TODO dynamic dylib loading
    ScreenSize size;
    size.Width = width;
    size.Height = height;
    return PolyCreateRenderingDevice(window, size);
}

void PolyEngineLoad(void* window, unsigned width, unsigned height)
{
    gConsole.LogError("Init engine load");
    engine = std::make_unique<Engine>();
    std::unique_ptr<IGame> game = std::unique_ptr<IGame>(LoadGame());
    std::unique_ptr<IRenderingDevice> device = std::unique_ptr<IRenderingDevice>(LoadRenderingDevice(window, width, height));

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
