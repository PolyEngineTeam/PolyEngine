find_package(RapidJSON REQUIRED)

set(POLYCORE_INCLUDE Src)

file(GLOB_RECURSE POLYCORE_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/Src *.cpp *.hpp *.h)
GenerateSourceGoups("${POLYCORE_SRCS}")

add_library(PolyCore SHARED ${POLYCORE_SRCS})
target_compile_options(PolyCore PRIVATE $<$<BOOL:${SIMD}>:${SIMD_FLAGS}>)
target_compile_definitions(PolyCore PRIVATE _CORE DISABLE_SIMD=$<NOT:$<BOOL:${SIMD}>>)
target_include_directories(PolyCore PUBLIC ${POLYCORE_INCLUDE})
target_link_libraries(PolyCore PRIVATE Rapid::JSON)

if(GENERATE_COVERAGE AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
	target_compile_options(PolyCore PRIVATE --coverage -fprofile-arcs -ftest-coverage)
	target_link_libraries(PolyCore PRIVATE -fprofile-arcs -ftest-coverage)
	add_custom_target(coverage_core
	                  COMMAND ${GCOV_COMMAND} -r -o "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/PolyCore.dir/Src" -s ${CMAKE_CURRENT_SOURCE_DIR} ${POLYCORE_SRCS}
	                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	                  COMMENT "Collecting gcov coverage information for PolyCore"
	)
endif(GENERATE_COVERAGE AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))

set_target_properties(PolyCore PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "Src/CorePCH.hpp")
cotire(PolyCore)
