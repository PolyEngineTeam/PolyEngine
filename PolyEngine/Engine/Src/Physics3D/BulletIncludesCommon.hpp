#pragma once

#include <Defines.hpp>

SILENCE_MSVC_WARNING(4305, "Surpressing msvc warnings in bullet.")
SILENCE_CLANG_WARNING(-Woverloaded-virtual, "Surpressing clang warnings in bullet.")

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

UNSILENCE_CLANG_WARNING()
UNSILENCE_MSVC_WARNING()