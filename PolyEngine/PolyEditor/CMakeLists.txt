# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOUIC ON)
SET(AUTOUIC_SEARCH_PATHS Form)
set(CMAKE_AUTOMOC ON)

if(WIN32)
elseif(APPLE)
    #TODO
else()
    find_package(OpenGL REQUIRED)
    find_package(X11 REQUIRED)
endif()

find_package(Qt5 COMPONENTS OpenGL Core Widgets X11Extras REQUIRED)


set(EDITOR_SRCS
        Src/EditorApp.cpp
        Src/GameplayViewportWidget.cpp
        Src/Main.cpp
        Src/PolyEditorPCH.cpp
        )
set(EDITOR_INCLUDE Src)
set(EDITOR_H_FOR_IDE
        Src/PolyEditorPCH.hpp
        Src/GameplayViewportWidget.hpp
        Src/EditorApp.hpp
        )

add_executable(polyeditor ${EDITOR_SRCS} ${AUTOGEN_SOURCE_GROUP} ${EDITOR_H_FOR_IDE})
target_include_directories(polyeditor PUBLIC ${EDITOR_INCLUDE})

if(WIN32)
elseif(APPLE)
    #TODO
else()
    target_link_libraries(polyeditor ${X11_LIBRARIES} ${OPENGL_LIBRARIES} ${CMAKE_DL_LIBS})
endif()

target_link_libraries(polyeditor Qt5::Widgets Qt5::OpenGL Qt5::Core Qt5::X11Extras polyengine polycore)
add_dependencies(polyeditor polyrenderingdevicegl) #dynamically loaded
qt5_use_modules(polyeditor OpenGL Widgets Core X11Extras)

cotire(polyeditor)