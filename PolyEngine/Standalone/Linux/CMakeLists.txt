find_package(X11 REQUIRED)
find_package(epoxy REQUIRED)

add_executable(polyrun main.cpp) #should we call it `polygo` instead? :P
target_include_directories(polyrun PRIVATE ${X11_INCLUDE_DIR})
target_link_libraries(polyrun PRIVATE polyengine ${X11_LIBRARIES} epoxy::gl ${CMAKE_DL_LIBS})
add_dependencies(polyrun polyrenderingdevicegl sgjgame) #dynamically loaded

cotire(polyrun)
