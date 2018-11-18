set(POLYTESTS_INCLUDE Src)

file(GLOB_RECURSE POLYTESTS_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
	${POLYTESTS_INCLUDE}/*.cpp 
	${POLYTESTS_INCLUDE}/*.hpp
	${POLYTESTS_INCLUDE}/*.h)
GenerateSourceGoups("${POLYTESTS_SRCS}")                                                          #

add_executable(${UNITTESTS_TARGET} ${POLYTESTS_SRCS})
add_dependencies(${UNITTESTS_TARGET} Prerequisites)
target_link_libraries(${UNITTESTS_TARGET} PRIVATE ${CORE_TARGET} ${ENGINE_TARGET} Catch)
ParseAndAddCatchTests(${UNITTESTS_TARGET})

# For calling "make tests"
add_custom_target(tests COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS ${UNITTESTS_TARGET})

# Coverage generation
if(GENERATE_COVERAGE AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
	add_custom_target(coverage)
	add_dependencies(coverage_core tests)
	add_dependencies(coverage_engine tests)
	add_dependencies(coverage coverage_core coverage_engine)
endif(GENERATE_COVERAGE AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))

cotire(${UNITTESTS_TARGET})
