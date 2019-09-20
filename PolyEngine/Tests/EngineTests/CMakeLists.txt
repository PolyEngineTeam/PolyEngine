set(ENGINE_TESTS_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/Src)

file(GLOB_RECURSE ENGINE_TESTS_SRCS RELATIVE ${CMAKE_CURRENT_LIST_DIR} 
	${ENGINE_TESTS_INCLUDE}/*.cpp 
	${ENGINE_TESTS_INCLUDE}/*.hpp
	${ENGINE_TESTS_INCLUDE}/*.h)
GenerateSourceGoups("${ENGINE_TESTS_SRCS}")                                                          #

add_executable(${ENGINE_TESTS_TARGET} ${ENGINE_TESTS_SRCS})
add_dependencies(${ENGINE_TESTS_TARGET} Prerequisites)
target_link_libraries(${ENGINE_TESTS_TARGET} PRIVATE ${ENGINE_TARGET} Catch)
ParseAndAddCatchTests(${ENGINE_TESTS_TARGET})

set_target_properties(${ENGINE_TESTS_TARGET} PROPERTIES FOLDER "Engine" )
