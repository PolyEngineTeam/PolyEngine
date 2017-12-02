find_package(SDL2 REQUIRED)

add_executable(polyrun Src/Main.cpp) #should we call it `polygo` instead? :P
target_include_directories(polyrun PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(polyrun PRIVATE polyengine polycore ${SDL2_LIBRARIES} ${CMAKE_DL_LIBS})
add_dependencies(polyrun polyrenderingdevicegl) #dynamically loaded

cotire(polyrun)
