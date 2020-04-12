#pragma once

#include <pe/Defines.hpp>

namespace pe::api {
class IEngine;

class IGame : public ::pe::core::BaseObject<>
{   
public:
    virtual void registerEngine(IEngine* engine) = 0;
    virtual void onStart() = 0;
    virtual void onEnd() = 0;
};

} // namespace pe::api

#define DECLARE_GAME() extern "C" { GAME_DLLEXPORT ::pe::api::IGame* POLY_STDCALL CreateGame(); }
#define DEFINE_GAME(Type) ::pe::api::IGame* POLY_STDCALL CreateGame() { return new Type(); }