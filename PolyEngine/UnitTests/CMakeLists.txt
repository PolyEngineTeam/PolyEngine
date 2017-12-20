set(POLYTESTS_SRCS
	Src/AABoxTests.cpp
	Src/AARectTests.cpp
	Src/AllocatorTests.cpp
	Src/AngleTests.cpp
	Src/BasicMathTests.cpp
	Src/ConfigTests.cpp
	Src/OrderedMapTests.cpp
	Src/DynarrayTests.cpp
	Src/EnumUtilsTests.cpp
	Src/MatrixTests.cpp
	Src/OptionalTests.cpp
	Src/QuaternionTests.cpp
	Src/QueueTests.cpp
	Src/ResourceManagerTests.cpp
	Src/RTTITests.cpp
	Src/SafePtrTests.cpp
	Src/StringTests.cpp
	Src/TransformComponentTests.cpp
	Src/UnsafeStorageTests.cpp
	Src/VectorTests.cpp
	Src/Vector2fTests.cpp
	Src/Vector2iTests.cpp
	Src/main.cpp
)

# Catch include
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${PROJECT_SOURCE_DIR}/ThirdParty/Catch/Include)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/ThirdParty/Catch/contrib)
include(ParseAndAddCatchTests)                                                            #

add_executable(polytests ${POLYTESTS_SRCS})
target_link_libraries(polytests PRIVATE polycore polyengine Catch)
ParseAndAddCatchTests(polytests)

# For calling "make tests"
add_custom_target(tests COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS polytests)

# Coverage generation
if(GENERATE_COVERAGE AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
	add_custom_target(coverage)
	add_dependencies(coverage_core tests)
	add_dependencies(coverage_engine tests)
	add_dependencies(coverage coverage_core coverage_engine)
endif(GENERATE_COVERAGE AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))

add_custom_command(
		TARGET polytests
		POST_BUILD
			# Inhouse DLL's
			COMMAND ${CMAKE_COMMAND} -E copy  "$<TARGET_FILE:polycore>" "$<TARGET_FILE_DIR:polytests>"
			COMMAND ${CMAKE_COMMAND} -E copy  "$<TARGET_FILE:polyengine>" "$<TARGET_FILE_DIR:polytests>"
			
			COMMENT "Copying Libs..." VERBATIM
		)

if(WIN32)
	# Copy all necessary DLL's on Windows
	add_custom_command(
		TARGET polytests
		POST_BUILD
			# Thirdparty DLL's
			COMMAND ${CMAKE_COMMAND} -E copy  "${CMAKE_SOURCE_DIR}/ThirdParty/SDL2/lib/x64/SDL2.dll" "$<TARGET_FILE_DIR:polytests>"
			COMMAND ${CMAKE_COMMAND} -E copy  "${CMAKE_SOURCE_DIR}/ThirdParty/Assimp/lib/Release/x64/assimp-vc140-mt.dll" "$<TARGET_FILE_DIR:polytests>"
			COMMAND ${CMAKE_COMMAND} -E copy  "${CMAKE_SOURCE_DIR}/ThirdParty/libepoxy/bin/x64/epoxy-0.dll" "$<TARGET_FILE_DIR:polytests>"

			COMMENT "Copying DLLs..." VERBATIM
		)
endif()

cotire(polytests)
