find_package(SDL2 REQUIRED)

if(APPLE)
	set(CMAKE_PREFIX_PATH /usr/local/opt/qt)
endif()

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOUIC ON)
SET(AUTOUIC_SEARCH_PATHS Form)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)


set(EDITOR_SRCS
        Src/CustomSDLWindow.cpp
        Src/EditorApp.cpp
        Src/PolyConsoleWidget.cpp
        Src/PolyMainWindow.cpp
        Src/PolyMainWindowUi.cpp
        Src/PolyViewportWidget.cpp
        Src/Main.cpp
        Src/PolyWindow.cpp
        Src/PolyEditorPCH.cpp
        )
set(EDITOR_INCLUDE Src)
set(EDITOR_H_FOR_IDE
        Src/CustomSDLWindow.hpp
        Src/EditorApp.hpp
        Src/PolyConsoleWidget.hpp
        Src/PolyMainWindow.hpp
        Src/PolyMainWindowUi.hpp
        Src/PolyViewportWidget.hpp
        Src/PolyWindow.hpp
        Src/PolyEditorPCH.hpp
        )

set(EDITOR_SRCS_FOR_APPLE Src-macOS/MacOSHelper.mm)
set(EDITOR_INCLUDE_FOR_APPLE Src-macOS)
set(EDITOR_H_FOR_IDE_FOR_APPLE Src-macOS/MacOSHelper.hpp)

add_executable(polyeditor ${EDITOR_SRCS} ${AUTOGEN_SOURCE_GROUP} ${EDITOR_H_FOR_IDE})
target_include_directories(polyeditor PUBLIC ${EDITOR_INCLUDE} ${SDL2_INCLUDE_DIRS})
target_link_libraries(polyeditor polyengine polycore ${SDL2_LIBRARIES} ${CMAKE_DL_LIBS})
target_link_libraries(polyeditor Qt5::Widgets Qt5::Core)
add_dependencies(polyeditor polyrenderingdevicegl) #dynamically loaded
qt5_use_modules(polyeditor Widgets Core)

if(APPLE)
    find_library(Cocoa_LIBRARY Cocoa)
    mark_as_advanced(Cocoa_LIBRARY)

    target_sources(polyeditor PRIVATE ${EDITOR_SRCS_FOR_APPLE} ${EDITOR_H_FOR_IDE_FOR_APPLE})
    target_include_directories(polyeditor PRIVATE ${EDITOR_INCLUDE_FOR_APPLE})
    target_link_libraries(polyeditor ${Cocoa_LIBRARY})
endif()

cotire(polyeditor)
