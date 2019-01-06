set(POLYENGINE_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/Src)

file(GLOB_RECURSE POLYENGINE_SRCS RELATIVE ${CMAKE_CURRENT_LIST_DIR} 
	${POLYENGINE_INCLUDE}/*.cpp 
	${POLYENGINE_INCLUDE}/*.hpp
	${POLYENGINE_INCLUDE}/*.h
)
GenerateSourceGoups("${POLYENGINE_SRCS}")

add_library(${ENGINE_TARGET} SHARED ${POLYENGINE_SRCS})
target_compile_definitions(${ENGINE_TARGET} PRIVATE _ENGINE)

target_include_directories(${ENGINE_TARGET} 
	PUBLIC 
			${POLYENGINE_INCLUDE} 
	PRIVATE 
			${BULLET_INCLUDE_DIRS}
			${STB_INCLUDE_DIRS} )

# Static libraries need to have all symbols exported in the shared library on *NIX systems.
if(LINUX)
	set(EXPORTED_LIBS -Wl,-whole-archive ${BOX2D_LIBRARIES} ${BULLET_LIBRARIES} -Wl,-no-whole-archive)
elseif(APPLE)
	set(EXPORTED_LIBS -Wl,-all_load ${BOX2D_LIBRARIES} ${BULLET_LIBRARIES} -Wl,-noall_load)
else()
	set(EXPORTED_LIBS ${BOX2D_LIBRARIES} ${BULLET_LIBRARIES})
endif()

target_link_libraries(${ENGINE_TARGET} PUBLIC ${CORE_TARGET} Imgui PRIVATE assimp freetype OpenAL ogg vorbis vorbisfile ${EXPORTED_LIBS})

set_target_properties(${ENGINE_TARGET} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "${CMAKE_CURRENT_LIST_DIR}/Src/EnginePCH.hpp")
set_target_properties(${ENGINE_TARGET} PROPERTIES FOLDER "Engine" )
cotire(${ENGINE_TARGET})
