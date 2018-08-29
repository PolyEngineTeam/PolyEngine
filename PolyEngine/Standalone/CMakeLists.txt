find_package(SDL2 REQUIRED)
find_package(assimp REQUIRED)
find_package(Freetype REQUIRED)
find_package(OpenAL REQUIRED)
find_package(OGG REQUIRED)
find_package(Vorbis REQUIRED)
find_package(Box2D REQUIRED)
find_package(Bullet REQUIRED)
find_package(RapidJSON REQUIRED)
find_package(STB REQUIRED)

set(STANDALONE_INCLUDE Src)

file(GLOB_RECURSE STANDALONE_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
	${STANDALONE_INCLUDE}/*.cpp 
	${STANDALONE_INCLUDE}/*.hpp
	${STANDALONE_INCLUDE}/*.h)
GenerateSourceGoups("${STANDALONE_SRCS}")

add_executable(PolyStandalone ${STANDALONE_SRCS}) #should we call it `polygo` instead? :P
target_include_directories(PolyStandalone PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(PolyStandalone PRIVATE PolyEngine PolyCore ass::imp Freetype::FT2 OpenAL::AL OGG::OGG Vorbis::Vorbis Vorbis::File Box2D::Box2D Rapid::JSON ${BULLET_LIBRARIES} ${SDL2_LIBRARIES} ${CMAKE_DL_LIBS} )
add_dependencies(PolyStandalone PolyRenderingDeviceGL) #dynamically loaded

cotire(PolyStandalone)
