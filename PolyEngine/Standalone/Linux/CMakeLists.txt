find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(X11 REQUIRED)

set(STANDALONE_SRCS main.cpp)

add_executable(standalone ${STANDALONE_SRCS})
target_include_directories(standalone PRIVATE ${OPENGL_INCLUDE_DIR} ${X11_INCLUDE_DIR})
target_link_libraries(standalone polycore polyengine polygldevice sgjgame ${OPENGL_LIBRARIES} GLEW::GLEW ${X11_LIBRARIES})
add_custom_command(TARGET standalone POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink "${PROJECT_SOURCE_DIR}/Engine/Res" $<TARGET_FILE_DIR:polyengine>/Res)

cotire(standalone)
