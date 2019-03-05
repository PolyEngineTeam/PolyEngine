set(POLYTESTS_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/Src)

file(GLOB_RECURSE POLYTESTS_SRCS RELATIVE ${CMAKE_CURRENT_LIST_DIR} 
	${POLYTESTS_INCLUDE}/*.cpp 
	${POLYTESTS_INCLUDE}/*.hpp
	${POLYTESTS_INCLUDE}/*.h)
GenerateSourceGoups("${POLYTESTS_SRCS}")                                                          #

add_executable(${UNITTESTS_TARGET} ${POLYTESTS_SRCS})
add_dependencies(${UNITTESTS_TARGET} Prerequisites)
target_link_libraries(${UNITTESTS_TARGET} PRIVATE ${CORE_TARGET} ${ENGINE_TARGET} ${ENGINEAPI_TARGET} Catch)
ParseAndAddCatchTests(${UNITTESTS_TARGET})

set_target_properties(${UNITTESTS_TARGET} PROPERTIES FOLDER "Engine" )

# For calling "make tests"
add_custom_target(tests COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS ${UNITTESTS_TARGET})
set_target_properties(tests PROPERTIES FOLDER "Engine/Misc" )
