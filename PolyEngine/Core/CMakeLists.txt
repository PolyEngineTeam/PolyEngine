set(POLYCORE_SRCS
	Src/AARect.cpp
	Src/Color.cpp
	Src/Logger.cpp
	Src/Matrix.cpp
	Src/Quaternion.cpp
	Src/SimdMath.cpp
	Src/UniqueID.cpp
	Src/Vector.cpp
)
set(POLYCORE_INCLUDE
	Src
)
set(POLYCORE_H_FOR_IDE
	Src/AARect.hpp
	Src/Allocator.hpp
	Src/Angle.hpp
	Src/BaseObject.hpp
	Src/BasicMath.hpp
	Src/Color.hpp
	Src/Core.hpp
	Src/CorePCH.hpp
	Src/Defines.hpp
	Src/Dynarray.hpp
	Src/EnumUtils.hpp
	Src/FileIO.hpp
	Src/IterablePoolAllocator.hpp
	Src/Logger.hpp
	Src/Matrix.hpp
	Src/PoolAllocator.hpp
	Src/Quaternion.hpp
	Src/Queue.hpp
	Src/SimdMath.hpp
	Src/String.hpp
	Src/UniqueID.hpp
	Src/Vector.hpp
)
add_library(polycore SHARED ${POLYCORE_SRCS} ${POLYCORE_H_FOR_IDE})
target_compile_definitions(polycore PRIVATE _CORE)
if (SIMD)
	target_compile_definitions(polycore PRIVATE DISABLE_SIMD=false)
else(SIMD)
	target_compile_definitions(polycore PRIVATE DISABLE_SIMD=true)
endif(SIMD)
target_include_directories(polycore INTERFACE ${POLYCORE_INCLUDE})
