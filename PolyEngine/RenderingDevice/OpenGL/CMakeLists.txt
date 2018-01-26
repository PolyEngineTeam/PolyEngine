find_package(SDL2 REQUIRED)
find_package(epoxy REQUIRED)

file(GLOB_RECURSE POLYGLDEVICE_SRCS Src/*.cpp)
set(POLYGLDEVICE_INCLUDE Src)
file(GLOB_RECURSE POLYGLDEVICE_H_FOR_IDE Src/*.hpp Src/*.h)

add_library(PolyRenderingDeviceGL SHARED ${POLYGLDEVICE_SRCS} ${POLYGLDEVICE_H_FOR_IDE})
target_compile_definitions(PolyRenderingDeviceGL PRIVATE _DEVICE)
target_include_directories(PolyRenderingDeviceGL PUBLIC ${POLYGLDEVICE_INCLUDE} ${SDL2_INCLUDE_DIRS})
target_link_libraries(PolyRenderingDeviceGL PRIVATE PolyEngine epoxy::gl ${SDL2_LIBRARIES})
set_target_properties(PolyRenderingDeviceGL PROPERTIES OUTPUT_NAME polyrenderingdevice)

cotire(PolyRenderingDeviceGL)
