set(POLYTESTS_SRCS
	Src/AABoxTests.cpp
	Src/AllocatorTests.cpp
	Src/AngleTests.cpp
	Src/OrderedMapTests.cpp
	Src/BasicMathTests.cpp
	Src/DynarrayTests.cpp
	Src/EnumUtilsTests.cpp
	Src/MatrixTests.cpp
	Src/OptionalTests.cpp
	Src/QuaternionTests.cpp
	Src/QueueTests.cpp
	Src/ResourceManagerTests.cpp
	Src/RTTITests.cpp
	Src/StringTests.cpp
	Src/TransformComponentTests.cpp
	Src/UnsafeStorageTests.cpp
	Src/VectorTests.cpp
	Src/Vector2fTests.cpp
	Src/Vector2iTests.cpp
	Src/main.cpp
)

if(NOT TARGET Catch)
	include(ExternalProject)
	if(IS_TRAVIS_BUILD)
		set(FETCH_EXTERNAL_CATCH GIT_REPOSITORY https://github.com/philsquared/Catch.git GIT_TAG 0c015aa8878db0b582afa95bb9ef8ebf97baf232)
	elseif(WIN32)
		set(FETCH_EXTERNAL_CATCH URL https://github.com/philsquared/Catch/archive/v1.8.2.zip URL_HASH MD5=d929768234f22382a32d9bfd76b32355)
	else()
		set(FETCH_EXTERNAL_CATCH URL https://github.com/philsquared/Catch/archive/v1.8.2.tar.gz URL_HASH MD5=23562b3d3c8cc0a191ca74580320429a)
	endif()
	ExternalProject_Add(
		Fetch-Catch
		PREFIX ${CMAKE_BINARY_DIR}/3rdparty/Catch
		${FETCH_EXTERNAL_CATCH}
		CONFIGURE_COMMAND ""
		BUILD_COMMAND ""
		INSTALL_COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/3rdparty/Catch/src/Fetch-Catch/single_include/catch.hpp
		                                         ${CMAKE_BINARY_DIR}/3rdparty/Catch/include/catch.hpp
		UPDATE_DISCONNECTED ON
		EXCLUDE_FROM_ALL    ON
	)

	add_library(Catch INTERFACE)
	if(NOT USE_BUNDLED_CATCH AND TARGET Fetch-Catch AND NOT CMAKE_VERSION VERSION_LESS 3.3)
		add_dependencies(Catch Fetch-Catch)
		target_include_directories(Catch INTERFACE ${CMAKE_BINARY_DIR}/3rdparty/Catch/include)
	else()
		target_include_directories(Catch INTERFACE ${PROJECT_SOURCE_DIR}/ThirdParty/Catch/Include)
	endif()
endif(NOT TARGET Catch)

add_executable(polytests ${POLYTESTS_SRCS})
target_link_libraries(polytests PRIVATE polycore polyengine Catch)
add_custom_target(tests COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS polytests)
#WORKAROUND(vuko): CTest tests do not depend on targets that produce COMMAND executables, so add a dummy test that will force a recompilation if needed
add_test(NAME "BUILD_TESTS"                                   COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target polytests --config "$<CONFIG>")
#TODO(vuko): one has to manually add a case here every time you add a new test to Catch... Maybe grep the files looking for TEST_CASE then generate?
add_test(NAME "AABox-contains"                                COMMAND polytests "AABox contains")
add_test(NAME "AABox-collisions-with-other-AABox"             COMMAND polytests "AABox collisions with other AABox")
add_test(NAME "AABox-intersection-calculation"                COMMAND polytests "AABox intersection calculation")
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
add_test(NAME "Vector2f-constructors"                         COMMAND polytests "Vector2f constructors")
add_test(NAME "Vector2f-comparison-operators"                 COMMAND polytests "Vector2f comparison operators")
add_test(NAME "Vector2f-Vector2f-operators"                   COMMAND polytests "Vector2f-Vector2f operators")
add_test(NAME "Vector2f-scalar-operators"                     COMMAND polytests "Vector2f-scalar operators")
add_test(NAME "Vector2f-algebraic-methods"                    COMMAND polytests "Vector2f algebraic methods")
add_test(NAME "Vector2i-comparison-operators"                 COMMAND polytests "Vector2i comparison operators")
add_test(NAME "Vector2i-Vector2i-operators"                   COMMAND polytests "Vector2i-Vector2i operators")
add_test(NAME "Vector2i-scalar-operators"                     COMMAND polytests "Vector2i-scalar operators")
add_test(NAME "Vector2i-algebraic-methods"                    COMMAND polytests "Vector2i algebraic methods")
add_test(NAME "Floating-point-equality-comparisons"           COMMAND polytests "Floating-point equality comparisons")
add_test(NAME "TransformComponent-with-no-parent"             COMMAND polytests "TransformComponent with no parent")
add_test(NAME "TransformComponent-with-parent"                COMMAND polytests "TransformComponent with parent")
add_test(NAME "Multi-layer-hierarchy"                         COMMAND polytests "Multi-layer hierarchy")
add_test(NAME "ResourceManager-loading-freeing"               COMMAND polytests "ResourceManager loading/freeing")

if(GENERATE_COVERAGE AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
	add_custom_target(coverage)
	add_dependencies(coverage_core tests)
	add_dependencies(coverage_engine tests)
	add_dependencies(coverage coverage_core coverage_engine)
endif(GENERATE_COVERAGE AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))

cotire(polytests)
