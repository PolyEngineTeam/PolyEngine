#find_package(RapidJSON REQUIRED)

set(POLYCORE_INCLUDE Src)

file(GLOB_RECURSE POLYCORE_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
	${POLYCORE_INCLUDE}/*.cpp 
	${POLYCORE_INCLUDE}/*.hpp
	${POLYCORE_INCLUDE}/*.h)
GenerateSourceGoups("${POLYCORE_SRCS}")

add_library(${CORE_TARGET} SHARED ${POLYCORE_SRCS})
target_compile_options(${CORE_TARGET} PRIVATE $<$<BOOL:${SIMD}>:${SIMD_FLAGS}>)
target_compile_definitions(${CORE_TARGET} PRIVATE _CORE DISABLE_SIMD=$<NOT:$<BOOL:${SIMD}>>)
target_include_directories(${CORE_TARGET} PUBLIC ${POLYCORE_INCLUDE} ${RapidJSON_INCLUDE_DIRS})

if(GENERATE_COVERAGE AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
	target_compile_options(${CORE_TARGET} PRIVATE --coverage -fprofile-arcs -ftest-coverage)
	target_link_libraries(${CORE_TARGET} PRIVATE -fprofile-arcs -ftest-coverage)
	add_custom_target(coverage_core
	                  COMMAND ${GCOV_COMMAND} -r -o "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${CORE_TARGET}.dir/Src" -s ${CMAKE_CURRENT_SOURCE_DIR} ${POLYCORE_SRCS}
	                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	                  COMMENT "Collecting gcov coverage information for ${CORE_TARGET}"
	)
endif(GENERATE_COVERAGE AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))

set_target_properties(${CORE_TARGET} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "Src/CorePCH.hpp")
cotire(${CORE_TARGET})