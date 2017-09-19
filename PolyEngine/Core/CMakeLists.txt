set(POLYCORE_SRCS
	Src/AABox.cpp
	Src/AARect.cpp
	Src/BaseObject.cpp
	Src/BinaryBuffer.cpp
	Src/Color.cpp
	Src/Logger.cpp
	Src/Matrix.cpp
	Src/Quaternion.cpp
	Src/RefCountedBase.cpp
	Src/RTTI.cpp
	Src/RTTITypeInfo.cpp
	Src/SimdMath.cpp
	Src/String.cpp
	Src/UniqueID.cpp
	Src/Vector.cpp
	Src/Vector2f.cpp
	Src/Vector2i.cpp
)
set(POLYCORE_INCLUDE Src)
set(POLYCORE_H_FOR_IDE
	Src/AABox.hpp
	Src/AARect.hpp
	Src/Allocator.hpp
	Src/Angle.hpp
	Src/OrderedMap.hpp
	Src/BTreePrimitives.hpp
	Src/BaseObject.hpp
	Src/BasicMath.hpp
	Src/BinaryBuffer.hpp
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
	Src/Optional.hpp
	Src/PoolAllocator.hpp
	Src/Quaternion.hpp
	Src/Queue.hpp
	Src/RefCountedBase.hpp
	Src/RTTI.hpp
	Src/RTTICast.hpp
	Src/RTTIProperty.hpp
	Src/RTTITypeInfo.hpp
	Src/RTTITypeInfoImpl.hpp
	Src/SimdMath.hpp
	Src/String.hpp
	Src/UniqueID.hpp
	Src/UnsafeStorage.hpp
	Src/Vector.hpp
	Src/Vector2f.hpp
	Src/Vector2i.hpp
)

add_library(polycore SHARED ${POLYCORE_SRCS} ${POLYCORE_H_FOR_IDE})
target_compile_options(polycore PRIVATE $<$<BOOL:${SIMD}>:${SIMD_FLAGS}>)
target_compile_definitions(polycore PRIVATE _CORE DISABLE_SIMD=$<NOT:$<BOOL:${SIMD}>>)
target_include_directories(polycore INTERFACE ${POLYCORE_INCLUDE})

if(GENERATE_COVERAGE AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
	target_compile_options(polycore PRIVATE --coverage -fprofile-arcs -ftest-coverage)
	target_link_libraries(polycore PRIVATE -fprofile-arcs -ftest-coverage)
	add_custom_target(coverage_core
	                  COMMAND ${GCOV_COMMAND} -r -o "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/polycore.dir/Src" -s ${CMAKE_CURRENT_SOURCE_DIR} ${POLYCORE_SRCS}
	                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	                  COMMENT "Collecting gcov coverage information for polycore"
	)
endif(GENERATE_COVERAGE AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))

set_target_properties(polycore PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "Src/CorePCH.hpp")
cotire(polycore)
