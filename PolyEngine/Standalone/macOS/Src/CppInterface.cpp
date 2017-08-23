#include "CppInterface.hpp"

#include <memory>

#include <Core.hpp>
#include <Engine.hpp>
#include <GLRenderingDevice.hpp>
#include <Game.hpp>

using namespace Poly;

static std::unique_ptr<Engine> engine;
static std::unique_ptr<IRenderingDevice> device;
static std::unique_ptr<IGame> game;

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
    game = std::unique_ptr<IGame>(LoadGame());
    device = std::unique_ptr<IRenderingDevice>(LoadRenderingDevice(window, width, height));

    engine->Init(std::move(game), std::move(device));
    gConsole.LogError("Engine loaded");
}

void PolyEngineUpdate()
{
    engine->Update();
}
