#include "CppInterface.hpp"

#include <memory>

#include <Core.hpp>
#include <Engine.hpp>
#include <dlfcn.h>

using namespace Poly;

#include <sstream>

extern "C" {
    using CreateRenderingDeviceFunc = Poly::IRenderingDevice* (void* window, const Poly::ScreenSize& size);
    using LoadGameFunc = Poly::IGame* ();
}

//-----------------------------------------------------------------------------------------------
struct RawFunc {
    void* libraryHandle;
    void* func;
};

//-----------------------------------------------------------------------------------------------
template<typename Function>
class DynLoaded {
public:
    //note(vuko): thankfully casting void* to func-ptr is defined and allowed in C++11 and up ._. UBSan still complains though :(
    DynLoaded() {}
    DynLoaded(RawFunc raw) : handle(raw.libraryHandle), func(reinterpret_cast<Function*>(raw.func)) {}
    DynLoaded(DynLoaded&& other) : DynLoaded{RawFunc{other.handle, reinterpret_cast<void*>(other.func)}} { other.handle = nullptr; other.func = nullptr; }
    DynLoaded& operator=(DynLoaded&& other) { handle = other.handle; func = other.func; other.handle = nullptr; other.func = nullptr; return *this; }
    ~DynLoaded() { if (handle) { dlclose(handle); } }
    template<typename... Args> inline typename std::result_of<Function*(Args...)>::type operator()(Args... args) { return func(std::forward<Args>(args)...); }
    inline bool FunctionValid() { return static_cast<bool>(func); }
private:
    void* handle = nullptr;
    Function* func = nullptr;
};

//-----------------------------------------------------------------------------------------------
RawFunc LoadFunction(const char* library, const char* functionName) {
    void* handle = dlopen(library, RTLD_NOW /*| RTLD_GLOBAL*/); //don't be lazy in resolving symbols /*and allow subsequently loaded libs to use them*/
    if (const char* err = dlerror()) { //we could simply check if the handle is null, but using dlerror() doubles as clearing error flags
        Poly::gConsole.LogError("{}", err);
        Poly::gConsole.LogError("Use a game run-script or set the DYLD_LIBRARY_PATH environment variable manually");
        return {nullptr, nullptr};
    }

    void* func = dlsym(handle, functionName);
    if(const char* err = dlerror()) { //symbols can be legally null, so we need to check for errors instead
        Poly::gConsole.LogError("{}", err);
        dlclose(handle);
        return {nullptr, nullptr};
    }

    return {handle, func};
}

//-----------------------------------------------------------------------------------------------
inline DynLoaded<CreateRenderingDeviceFunc> GetRenderingDeviceCreator(const char* soname) {
    return {LoadFunction(soname, "PolyCreateRenderingDevice")};
}

//-----------------------------------------------------------------------------------------------
inline DynLoaded<LoadGameFunc> GetGameLoader(const char* soname) {
    return {LoadFunction(soname, "CreateGame")};
}

//-----------------------------------------------------------------------------------------------
static std::unique_ptr<Engine> engine;
static DynLoaded<CreateRenderingDeviceFunc> renderingDeviceDylibHandle;
static DynLoaded<LoadGameFunc> gameDylibHandle;

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
    gameDylibHandle = GetGameLoader("libgame.dylib"); //the name could be passed as a command-line arg
    if (gameDylibHandle.FunctionValid())
        game.reset(gameDylibHandle());
    ASSERTE(game, "Game load failed!");

    // Load rendering device dylib
    std::unique_ptr<IRenderingDevice> device;
    renderingDeviceDylibHandle = GetRenderingDeviceCreator("libpolyrenderingdevice.dylib");
    if (renderingDeviceDylibHandle.FunctionValid())
         device.reset(renderingDeviceDylibHandle(window, size));
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
