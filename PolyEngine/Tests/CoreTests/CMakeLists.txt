set(CORE_TESTS_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/Src)

file(GLOB_RECURSE CORE_TESTS_SRCS RELATIVE ${CMAKE_CURRENT_LIST_DIR} 
	${CORE_TESTS_INCLUDE}/*.cpp 
	${CORE_TESTS_INCLUDE}/*.hpp
	${CORE_TESTS_INCLUDE}/*.h)
GenerateSourceGoups("${CORE_TESTS_SRCS}")                                                          #

add_executable(${CORE_TESTS_TARGET} ${CORE_TESTS_SRCS})
add_dependencies(${CORE_TESTS_TARGET} Prerequisites)
target_link_libraries(${CORE_TESTS_TARGET} PRIVATE ${CORE_TARGET} Catch)
ParseAndAddCatchTests(${CORE_TESTS_TARGET})

set_target_properties(${CORE_TESTS_TARGET} PROPERTIES FOLDER "Engine" )
