find_package(SDL2 REQUIRED)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOUIC ON)
SET(AUTOUIC_SEARCH_PATHS Form)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)


file(GLOB_RECURSE EDITOR_SRCS Src/*.cpp)
set(EDITOR_INCLUDE Src)
file(GLOB_RECURSE EDITOR_H_FOR_IDE Src/*.hpp Src/*.h)

file(GLOB_RECURSE EDITOR_SRCS_FOR_APPLE Src-macOS/*.cpp Src-macOS/*.mm)
set(EDITOR_INCLUDE_FOR_APPLE Src-macOS)
file(GLOB_RECURSE EDITOR_H_FOR_IDE_FOR_APPLE Src-macOS/*.hpp Src-macOS/*.h)

add_executable(PolyEditor ${EDITOR_SRCS} ${AUTOGEN_SOURCE_GROUP} ${EDITOR_H_FOR_IDE})
target_include_directories(PolyEditor PUBLIC ${EDITOR_INCLUDE} ${SDL2_INCLUDE_DIRS})
target_link_libraries(PolyEditor PolyEngine PolyCore ${SDL2_LIBRARIES} ${CMAKE_DL_LIBS})
target_link_libraries(PolyEditor Qt5::Widgets Qt5::Core)
add_dependencies(PolyEditor PolyRenderingDeviceGL) #dynamically loaded
qt5_use_modules(PolyEditor Widgets Core)

if(APPLE)
    find_library(Cocoa_LIBRARY Cocoa)
    mark_as_advanced(Cocoa_LIBRARY)

    target_sources(PolyEditor PRIVATE ${EDITOR_SRCS_FOR_APPLE} ${EDITOR_H_FOR_IDE_FOR_APPLE})
    target_include_directories(PolyEditor PRIVATE ${EDITOR_INCLUDE_FOR_APPLE})
    target_link_libraries(PolyEditor ${Cocoa_LIBRARY})
endif()

# Strange bug in cotire for macOS. Disable it for now. Include paths are not working.
if(NOT APPLE)
    cotire(PolyEditor)
endif()
