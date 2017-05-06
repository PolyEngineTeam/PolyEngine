set(POLYTESTS_SRCS
	Src/AllocatorTests.cpp
	Src/AngleTests.cpp
	Src/BasicMathTests.cpp
	Src/DynarrayTests.cpp
	Src/EnumUtilsTests.cpp
	Src/main.cpp
	Src/MatrixTests.cpp
	Src/QuaternionTests.cpp
	Src/QueueTests.cpp
	Src/VectorTests.cpp
)

if(NOT TARGET Catch) #TODO(vuko): what about our embedded copy of Catch? How to integrate sanely? And should we?
	include(ExternalProject)
	if(IS_TRAVIS_BUILD)
		set(FETCH_EXTERNAL_CATCH GIT_REPOSITORY https://github.com/philsquared/Catch.git GIT_TAG 0c015aa8878db0b582afa95bb9ef8ebf97baf232)
	elseif(WIN32)
		set(FETCH_EXTERNAL_CATCH URL https://github.com/philsquared/Catch/archive/v1.8.2.zip URL_HASH MD5=d929768234f22382a32d9bfd76b32355)
	else()
		set(FETCH_EXTERNAL_CATCH URL https://github.com/philsquared/Catch/archive/v1.8.2.tar.gz URL_HASH MD5=23562b3d3c8cc0a191ca74580320429a)
	endif()
	ExternalProject_Add(
		Catch-External
		PREFIX ${CMAKE_BINARY_DIR}/external/Catch
		${FETCH_EXTERNAL_CATCH}
		CONFIGURE_COMMAND ""
		BUILD_COMMAND ""
		INSTALL_COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/external/Catch/src/Catch-External/single_include/catch.hpp
													${CMAKE_BINARY_DIR}/external/Catch/include/catch.hpp
	)
	add_library(Catch INTERFACE)
	add_dependencies(Catch Catch-External)
	target_include_directories(Catch INTERFACE ${CMAKE_BINARY_DIR}/external/Catch/include)
endif(NOT TARGET Catch)

add_executable(polytests ${POLYTESTS_SRCS})
target_link_libraries(polytests polycore polyengine Catch)
#TODO(vuko): you have to manually add a case here every time you add a new test to Catch... Maybe grep the files looking for TEST_CASE then generate?
add_test(NAME "Pool-allocator"                                COMMAND polytests "Pool allocator")
add_test(NAME "Iterable-pool-allocator"                       COMMAND polytests "Iterable pool allocator")
add_test(NAME "Angle-constructors"                            COMMAND polytests "Angle constructors")
add_test(NAME "Comparison-operators"                          COMMAND polytests "Comparison operators")
add_test(NAME "Dynarray-constructors"                         COMMAND polytests "Dynarray constructors")
add_test(NAME "Dynarray-assign-operator"                      COMMAND polytests "Dynarray assign operator")
add_test(NAME "Dynarray-comparison-operators"                 COMMAND polytests "Dynarray comparison operators")
add_test(NAME "Dynarray-iterators"                            COMMAND polytests "Dynarray iterators")
add_test(NAME "Dynarray-basics"                               COMMAND polytests "Dynarray basics")
add_test(NAME "Dynarray-with-BaseObject"                      COMMAND polytests "Dynarray with BaseObject")
add_test(NAME "EnumArray-tests"                               COMMAND polytests "EnumArray tests")
add_test(NAME "Matrix-constructors"                           COMMAND polytests "Matrix constructors")
add_test(NAME "Matrix-comparison-operators"                   COMMAND polytests "Matrix comparison operators")
add_test(NAME "Matrix-Matrix-multiplication-operators"        COMMAND polytests "Matrix-Matrix multiplication operators")
add_test(NAME "Matrx-Vector-multiplication-operator"          COMMAND polytests "Matrx-Vector multiplication operator")
add_test(NAME "Matrix-algebraic-methods"                      COMMAND polytests "Matrix algebraic methods")
add_test(NAME "Matrix-set-methods"                            COMMAND polytests "Matrix set methods")
add_test(NAME "Matrix-decomposition"                          COMMAND polytests "Matrix decomposition")
add_test(NAME "Quaternion-constructors"                       COMMAND polytests "Quaternion constructors")
add_test(NAME "Quaternion-comparison-operators"               COMMAND polytests "Quaternion comparison operators")
add_test(NAME "Quaternion-Quaternion-multiplication-operator" COMMAND polytests "Quaternion-Quaternion multiplication operator")
add_test(NAME "Quaternion-algerbraic-methods"                 COMMAND polytests "Quaternion algerbraic methods")
add_test(NAME "Queue-tests"                                   COMMAND polytests "Queue tests")
add_test(NAME "Queue-tests-with-BaseObject"                   COMMAND polytests "Queue tests (with BaseObject)")
add_test(NAME "Vector-constructors"                           COMMAND polytests "Vector constructors")
add_test(NAME "Vector-comparison-operators"                   COMMAND polytests "Vector comparison operators")
add_test(NAME "Vector-Vector-operators"                       COMMAND polytests "Vector-Vector operators")
add_test(NAME "Vector-scalar-operators"                       COMMAND polytests "Vector-scalar operators")
add_test(NAME "Vector-algebraic-methods"                      COMMAND polytests "Vector algebraic methods")
add_test(NAME "Floating-point-equality-comparisons"           COMMAND polytests "Floating-point equality comparisons")

if(GENERATE_COVERAGE AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")) #TODO(vuko): clean this up, merge coverage-core and coverage-engine maybe?
	target_compile_options(polycore PRIVATE --coverage -fprofile-arcs -ftest-coverage)
	target_compile_options(polycore PRIVATE --coverage -fprofile-arcs -ftest-coverage)
	target_link_libraries(polyengine PRIVATE -fprofile-arcs -ftest-coverage)
	target_link_libraries(polyengine PRIVATE -fprofile-arcs -ftest-coverage)
	add_custom_target(test-coverage-core
		COMMAND ${GCOV_COMMAND} -o CMakeFiles/polycore.dir/Src ${POLYCORE_SRCS} #TODO(vuko): hardcoded path
		WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
		COMMENT "Collecting gcov coverage information"
	)
	add_custom_target(test-coverage-engine
		COMMAND ${GCOV_COMMAND} -o CMakeFiles/polyengine.dir/Src ${POLYENGINE_SRCS}
		WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
		COMMENT "Collecting gcov coverage information"
	)
endif(GENERATE_COVERAGE AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
