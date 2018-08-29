find_package(SDL2 REQUIRED)
find_package(epoxy REQUIRED)
find_package(assimp REQUIRED)
find_package(Freetype REQUIRED)
find_package(OpenAL REQUIRED)
find_package(OGG REQUIRED)
find_package(Vorbis REQUIRED)
find_package(Box2D REQUIRED)
find_package(Bullet REQUIRED)
find_package(RapidJSON REQUIRED)
find_package(STB REQUIRED)

set(POLYGLDEVICE_INCLUDE Src)

file(GLOB_RECURSE POLYGLDEVICE_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
	${POLYGLDEVICE_INCLUDE}/*.cpp 
	${POLYGLDEVICE_INCLUDE}/*.hpp
	${POLYGLDEVICE_INCLUDE}/*.h)
GenerateSourceGoups("${POLYGLDEVICE_SRCS}")

add_library(PolyRenderingDeviceGL SHARED ${POLYGLDEVICE_SRCS})
target_compile_definitions(PolyRenderingDeviceGL PRIVATE _DEVICE)
target_include_directories(PolyRenderingDeviceGL PUBLIC ${POLYGLDEVICE_INCLUDE} ${SDL2_INCLUDE_DIRS} ${BULLET_INCLUDE_DIRS} ${STB_INCLUDE_DIRS})
target_link_libraries(PolyRenderingDeviceGL PRIVATE PolyEngine epoxy::gl ass::imp Freetype::FT2 OpenAL::AL OGG::OGG Vorbis::Vorbis Vorbis::File Box2D::Box2D Rapid::JSON ${BULLET_LIBRARIES} ${SDL2_LIBRARIES})
set_target_properties(PolyRenderingDeviceGL PROPERTIES OUTPUT_NAME polyrenderingdevice)

set_target_properties(PolyRenderingDeviceGL PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "Src/PolyRenderingDeviceGLPCH.hpp")
cotire(PolyRenderingDeviceGL)
