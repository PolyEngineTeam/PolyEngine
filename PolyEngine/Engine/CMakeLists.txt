find_package(assimp REQUIRED)
find_package(Freetype REQUIRED)
find_package(OpenAL REQUIRED)
find_package(OGG REQUIRED)
find_package(Vorbis REQUIRED)
find_package(Box2D REQUIRED)
find_package(Bullet REQUIRED)
find_package(RapidJSON REQUIRED)
find_package(STB REQUIRED)

set(POLYENGINE_INCLUDE Src)

file(GLOB_RECURSE POLYENGINE_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
	${POLYENGINE_INCLUDE}/*.cpp 
	${POLYENGINE_INCLUDE}/*.hpp
	${POLYENGINE_INCLUDE}/*.h)
GenerateSourceGoups("${POLYENGINE_SRCS}")


add_library(PolyEngine SHARED ${POLYENGINE_SRCS})
target_compile_definitions(PolyEngine PRIVATE _ENGINE)
target_include_directories(PolyEngine PUBLIC ${POLYENGINE_INCLUDE} ${BULLET_INCLUDE_DIRS} ${STB_INCLUDE_DIRS})
target_link_libraries(PolyEngine PUBLIC PolyCore PRIVATE ass::imp Freetype::FT2 OpenAL::AL OGG::OGG Vorbis::Vorbis Vorbis::File Box2D::Box2D Rapid::JSON ${BULLET_LIBRARIES})

if(GENERATE_COVERAGE AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
	target_compile_options(PolyEngine PRIVATE --coverage -fprofile-arcs -ftest-coverage)
	target_link_libraries(PolyEngine PRIVATE -fprofile-arcs -ftest-coverage)
	add_custom_target(coverage_engine
	                  COMMAND ${GCOV_COMMAND} -r -o "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/PolyEngine.dir/Src" -s ${CMAKE_CURRENT_SOURCE_DIR} ${POLYENGINE_SRCS}
	                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	                  COMMENT "Collecting gcov coverage information for PolyEngine"
	)
endif(GENERATE_COVERAGE AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))

set_target_properties(PolyEngine PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "Src/EnginePCH.hpp")
cotire(PolyEngine)
