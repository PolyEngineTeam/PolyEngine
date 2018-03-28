find_package(SDL2 REQUIRED)

set(STANDALONE_INCLUDE Src)

file(GLOB_RECURSE STANDALONE_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
	${STANDALONE_INCLUDE}/*.cpp 
	${STANDALONE_INCLUDE}/*.hpp
	${STANDALONE_INCLUDE}/*.h)
GenerateSourceGoups("${STANDALONE_SRCS}")

add_executable(PolyStandalone ${STANDALONE_SRCS}) #should we call it `polygo` instead? :P
target_include_directories(PolyStandalone PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(PolyStandalone PRIVATE PolyEngine PolyCore ${SDL2_LIBRARIES} ${CMAKE_DL_LIBS})
add_dependencies(PolyStandalone PolyRenderingDeviceGL) #dynamically loaded

cotire(PolyStandalone)
