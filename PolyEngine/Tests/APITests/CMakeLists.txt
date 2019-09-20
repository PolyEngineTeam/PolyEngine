set(API_TESTS_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/Src)

file(GLOB_RECURSE API_TESTS_SRCS RELATIVE ${CMAKE_CURRENT_LIST_DIR} 
	${API_TESTS_INCLUDE}/*.cpp 
	${API_TESTS_INCLUDE}/*.hpp
	${API_TESTS_INCLUDE}/*.h)
GenerateSourceGoups("${API_TESTS_SRCS}")                                                          #

add_executable(${API_TESTS_TARGET} ${API_TESTS_SRCS})
add_dependencies(${API_TESTS_TARGET} Prerequisites)
target_link_libraries(${API_TESTS_TARGET} PRIVATE ${API_TARGET} Catch)
ParseAndAddCatchTests(${API_TESTS_TARGET})

set_target_properties(${API_TESTS_TARGET} PROPERTIES FOLDER "Engine" )
