#pragma once

#include <pe/Defines.hpp>

// Workaround for not working warning silencing on GCC in PCH (Probably cotire related)
// @fixme
#if defined(GCC_COMPILER)
    #pragma GCC system_header
#endif

// Core
#include <pe/core/rtti/RTTI.hpp>

// Math
#include <pe/core/math/BasicMath.hpp>
#include <pe/core/math/Angle.hpp>
#include <pe/core/math/Vector.hpp>
#include <pe/core/math/Vector2f.hpp>
#include <pe/core/math/Vector2i.hpp>
#include <pe/core/math/Vector3f.hpp>
#include <pe/core/math/Matrix.hpp>
#include <pe/core/math/Quaternion.hpp>
#include <pe/core/math/SimdMath.hpp>
#include <pe/core/math/Random.hpp>
#include <pe/core/math/Frustum.hpp>

// Geometry
#include <pe/core/math/AABox.hpp>
#include <pe/core/math/AARect.hpp>

// Memory
#include <pe/core/BaseObject.hpp>
#include <pe/core/memory/Allocator.hpp>
#include <pe/core/memory/PoolAllocator.hpp>
#include <pe/core/memory/IterablePoolAllocator.hpp>
#include <pe/core/memory/RefCountedBase.hpp>
#include <pe/core/memory/SafePtr.hpp>
#include <pe/core/memory/SafePtrRoot.hpp>

// Containers
#include <pe/core/storage/String.hpp>
#include <pe/core/storage/StringBuilder.hpp>
#include <pe/core/storage/Queue.hpp>
#include <pe/core/storage/PriorityQueue.hpp>


// Other
#include <pe/core/math/Color.hpp>
#include <pe/core/utils/FileIO.hpp>
#include <pe/core/utils/Logger.hpp>
#include <pe/core/UniqueID.hpp>
#include <pe/core/utils/EnumUtils.hpp>
#include <pe/core/utils/OutputStream.hpp> 

// OpenAL
#include <AL/al.h>
#include <AL/alc.h>

// Sound
#include <ogg/ogg.h>
SILENCE_GCC_WARNING(-Wunused-variable, "Surpressing gcc vorbis warnings")
#include <vorbis/vorbisfile.h>
UNSILENCE_GCC_WARNING()

// Box2D
#include <Box2D/Box2D.h>

// Bullet
SILENCE_MSVC_WARNING(4305, "Surpressing msvc warnings in bullet.")
SILENCE_CLANG_WARNING(-Woverloaded-virtual, "Surpressing clang warnings in bullet.")
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
UNSILENCE_CLANG_WARNING()
UNSILENCE_MSVC_WARNING()

// Freetype
#include <ft2build.h>
#include FT_FREETYPE_H

// Assimp
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include <imgui.h>

// Temporary namespace usings
using namespace pe;