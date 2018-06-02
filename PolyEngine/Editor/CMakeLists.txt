find_package(SDL2 REQUIRED)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOUIC ON)
SET(AUTOUIC_SEARCH_PATHS Form)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

set(EDITOR_INCLUDE Src)

file(GLOB_RECURSE EDITOR_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
    ${EDITOR_INCLUDE}/*.cpp 
    ${EDITOR_INCLUDE}/*.hpp 
    ${EDITOR_INCLUDE}/*.h)
GenerateSourceGoups("${EDITOR_SRCS}")

add_executable(PolyEditor ${EDITOR_SRCS} ${AUTOGEN_SOURCE_GROUP})
target_include_directories(PolyEditor PUBLIC ${EDITOR_INCLUDE} ${SDL2_INCLUDE_DIRS})
target_link_libraries(PolyEditor PolyEngine PolyCore ${SDL2_LIBRARIES} ${CMAKE_DL_LIBS})
target_link_libraries(PolyEditor Qt5::Widgets Qt5::Core)
add_dependencies(PolyEditor PolyRenderingDeviceGL) #dynamically loaded
qt5_use_modules(PolyEditor Widgets Core)

if(APPLE)
    set(EDITOR_INCLUDE_FOR_APPLE Src-macOS)

    file(GLOB_RECURSE EDITOR_SRCS_FOR_APPLE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
        ${EDITOR_INCLUDE_FOR_APPLE}/*.mm
        ${EDITOR_INCLUDE_FOR_APPLE}/*.cpp
        ${EDITOR_INCLUDE_FOR_APPLE}/*.hpp
        ${EDITOR_INCLUDE_FOR_APPLE}/*.h)
    GenerateSourceGoups("${EDITOR_SRCS_FOR_APPLE}")

    find_library(Cocoa_LIBRARY Cocoa)
    mark_as_advanced(Cocoa_LIBRARY)

    target_sources(PolyEditor PRIVATE ${EDITOR_SRCS_FOR_APPLE})
    target_include_directories(PolyEditor PRIVATE ${EDITOR_INCLUDE_FOR_APPLE})
    target_link_libraries(PolyEditor ${Cocoa_LIBRARY})
endif()

# Strange bug in cotire for macOS. Disable it for now. Include paths are not working.
if(NOT APPLE)
    cotire(PolyEditor)
endif()
