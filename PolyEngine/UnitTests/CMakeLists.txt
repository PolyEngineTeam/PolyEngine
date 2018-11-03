set(POLYTESTS_INCLUDE Src)

file(GLOB_RECURSE POLYTESTS_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
	${POLYTESTS_INCLUDE}/*.cpp 
	${POLYTESTS_INCLUDE}/*.hpp
	${POLYTESTS_INCLUDE}/*.h)
GenerateSourceGoups("${POLYTESTS_SRCS}")

# Catch include
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${PROJECT_SOURCE_DIR}/ThirdParty/Catch/Include)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/ThirdParty/Catch/contrib)
include(ParseAndAddCatchTests)                                                            #

add_executable(${UNITTESTS_TARGET} ${POLYTESTS_SRCS})
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

add_custom_command(
		TARGET ${UNITTESTS_TARGET}
		POST_BUILD
			# Inhouse shared library dependancies
			COMMAND ${CMAKE_COMMAND} -E copy  "$<TARGET_FILE:${CORE_TARGET}>" "$<TARGET_FILE_DIR:${UNITTESTS_TARGET}>"
			COMMAND ${CMAKE_COMMAND} -E copy  "$<TARGET_FILE:${ENGINE_TARGET}>" "$<TARGET_FILE_DIR:${UNITTESTS_TARGET}>"
			
			COMMENT "Copying Libs..." VERBATIM
		)

if(WIN32)
	# Copy all necessary DLL's on Windows
	add_custom_command(
		TARGET ${UNITTESTS_TARGET}
		POST_BUILD
			# Thirdparty DLL's
			COMMAND ${CMAKE_COMMAND} -E copy  "${CMAKE_SOURCE_DIR}/ThirdParty/SDL2/lib/x64/SDL2.dll" "$<TARGET_FILE_DIR:${UNITTESTS_TARGET}>"
			COMMAND ${CMAKE_COMMAND} -E copy  "${CMAKE_SOURCE_DIR}/ThirdParty/Assimp/lib/Release/x64/assimp-vc140-mt.dll" "$<TARGET_FILE_DIR:${UNITTESTS_TARGET}>"
			COMMAND ${CMAKE_COMMAND} -E copy  "${CMAKE_SOURCE_DIR}/ThirdParty/libepoxy/bin/x64/epoxy-0.dll" "$<TARGET_FILE_DIR:${UNITTESTS_TARGET}>"

			COMMENT "Copying DLLs..." VERBATIM
		)
endif()

cotire(${UNITTESTS_TARGET})

CopyArtifacts(${UNITTESTS_TARGET})
