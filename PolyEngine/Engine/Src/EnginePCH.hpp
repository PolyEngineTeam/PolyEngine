#pragma once

#include <Defines.hpp>

// Workaround for not working warning silencing on GCC in PCH (Probably cotire related)
// @fixme
#if defined(GCC_COMPILER)
    #pragma GCC system_header
#endif

// Core
#include <RTTI/RTTI.hpp>

// Math
#include <Math/BasicMath.hpp>
#include <Math/Angle.hpp>
#include <Math/Vector.hpp>
#include <Math/Vector2f.hpp>
#include <Math/Vector2i.hpp>
#include <Math/Vector3f.hpp>
#include <Math/Matrix.hpp>
#include <Math/Quaternion.hpp>
#include <Math/SimdMath.hpp>
#include <Math/Random.hpp>
#include <Math/Frustum.hpp>

// Geometry
#include <Math/AABox.hpp>
#include <Math/AARect.hpp>

// Memory
#include <BaseObject.hpp>
#include <Memory/Allocator.hpp>
#include <Memory/PoolAllocator.hpp>
#include <Memory/IterablePoolAllocator.hpp>
#include <Memory/RefCountedBase.hpp>
#include <Memory/SafePtr.hpp>
#include <Memory/SafePtrRoot.hpp>

// Containers
#include <Collections/String.hpp>
#include <Collections/StringBuilder.hpp>
#include <Collections/Dynarray.hpp>
#include <Collections/Queue.hpp>
#include <Collections/PriorityQueue.hpp>
#include <Collections/OrderedMap.hpp>

// Other
#include <Math/Color.hpp>
#include <Utils/FileIO.hpp>
#include <Utils/Logger.hpp>
#include <UniqueID.hpp>
#include <Utils/EnumUtils.hpp>
#include <Utils/OutputStream.hpp> 

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