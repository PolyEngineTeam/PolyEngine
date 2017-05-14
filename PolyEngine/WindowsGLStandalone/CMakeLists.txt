find_package(OpenGL REQUIRED)
list(APPEND CMAKE_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/ThirdParty/glew/include")
list(APPEND CMAKE_LIBRARY_PATH "${CMAKE_SOURCE_DIR}/ThirdParty/glew/lib/Release/x64/" "${CMAKE_SOURCE_DIR}/ThirdParty/glew/lib/Release/Win32/")
find_package(GLEW REQUIRED)

set(STANDALONE_SRCS Src/Main.cpp)

add_executable(standalone ${STANDALONE_SRCS})
target_include_directories(standalone PRIVATE ${OPENGL_INCLUDE_DIR})
set_target_properties(standalone PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
target_link_libraries(standalone polycore polyengine testgame ${OPENGL_LIBRARIES} GLEW::GLEW)
add_custom_command(
		TARGET standalone POST_BUILD
		COMMAND
		${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/ThirdParty/glew/bin/Release/$<${ARCH_X64}:x64>$<${ARCH_X86}:Win32>/glew32.dll"
		                                      "$<TARGET_FILE_DIR:standalone>"
		COMMAND
		${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/ThirdParty/assimp/lib/Release/$<${ARCH_X64}:x64>$<${ARCH_X86}:Win32>/assimp-vc140-mt.dll"
		                                      "$<TARGET_FILE_DIR:standalone>"
)

cotire(standalone)
