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


add_library(${ENGINE_TARGET} SHARED ${POLYENGINE_SRCS})
target_compile_definitions(${ENGINE_TARGET} PRIVATE _ENGINE)
target_include_directories(${ENGINE_TARGET} PUBLIC ${POLYENGINE_INCLUDE} ${BULLET_INCLUDE_DIRS} ${STB_INCLUDE_DIRS})
target_link_libraries(${ENGINE_TARGET} PUBLIC ${CORE_TARGET} PRIVATE ass::imp Freetype::FT2 OpenAL::AL OGG::OGG Vorbis::Vorbis Vorbis::File Box2D::Box2D Rapid::JSON ${BULLET_LIBRARIES})

if(GENERATE_COVERAGE AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
	target_compile_options(${ENGINE_TARGET} PRIVATE --coverage -fprofile-arcs -ftest-coverage)
	target_link_libraries(${ENGINE_TARGET} PRIVATE -fprofile-arcs -ftest-coverage)
	add_custom_target(coverage_engine
	                  COMMAND ${GCOV_COMMAND} -r -o "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${ENGINE_TARGET}.dir/Src" -s ${CMAKE_CURRENT_SOURCE_DIR} ${POLYENGINE_SRCS}
	                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	                  COMMENT "Collecting gcov coverage information for ${ENGINE_TARGET}"
	)
endif(GENERATE_COVERAGE AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))

set_target_properties(${ENGINE_TARGET} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "Src/EnginePCH.hpp")
cotire()

CopyArtifacts(${ENGINE_TARGET})
