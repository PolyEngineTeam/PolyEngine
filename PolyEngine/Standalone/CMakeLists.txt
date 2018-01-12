find_package(SDL2 REQUIRED)

add_executable(PolyStandalone Src/Main.cpp) #should we call it `polygo` instead? :P
target_include_directories(PolyStandalone PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(PolyStandalone PRIVATE PolyEngine PolyCore ${SDL2_LIBRARIES} ${CMAKE_DL_LIBS})
add_dependencies(PolyStandalone PolyRenderingDeviceGL) #dynamically loaded

cotire(PolyStandalone)
