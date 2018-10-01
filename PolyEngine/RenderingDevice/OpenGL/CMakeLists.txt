find_package(SDL2 REQUIRED)
find_package(epoxy REQUIRED)

set(POLYGLDEVICE_INCLUDE Src)

file(GLOB_RECURSE POLYGLDEVICE_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
	${POLYGLDEVICE_INCLUDE}/*.cpp 
	${POLYGLDEVICE_INCLUDE}/*.hpp
	${POLYGLDEVICE_INCLUDE}/*.h)
GenerateSourceGoups("${POLYGLDEVICE_SRCS}")

add_library(PolyRenderingDeviceGL SHARED ${POLYGLDEVICE_SRCS})
target_compile_definitions(PolyRenderingDeviceGL PRIVATE _DEVICE)
target_include_directories(PolyRenderingDeviceGL PUBLIC ${POLYGLDEVICE_INCLUDE} ${SDL2_INCLUDE_DIRS})
target_link_libraries(PolyRenderingDeviceGL PRIVATE PolyEngine epoxy::gl ${SDL2_LIBRARIES})

#set_target_properties(PolyRenderingDeviceGL PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "Src/PolyRenderingDeviceGLPCH.hpp")
cotire(PolyRenderingDeviceGL)

CopyArtifacts(PolyRenderingDeviceGL)