find_package(OpenGL REQUIRED)
if(WIN32)
	list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/ThirdParty/glew")
else(WIN32)
	find_package(X11 REQUIRED)
endif(WIN32)
find_package(GLEW REQUIRED)


if(WIN32)
	set(STANDALONE_SRCS
		Src/Main.cpp
	)
else(WIN32)
	set(STANDALONE_SRCS
		main.cpp
	)
endif(WIN32)

add_executable(standalone ${STANDALONE_SRCS})
if(WIN32)
	target_include_directories(standalone BEFORE PRIVATE "${CMAKE_SOURCE_DIR}/ThirdParty/glew")

	if(ARCH_X64)
		add_custom_command(TARGET standalone POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/ThirdParty/glew/bin/Release/x64/glew32.dll" $<TARGET_FILE_DIR:standalone>)
	else(ARCH_X64)
		add_custom_command(TARGET standalone POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/ThirdParty/glew/bin/Release/Win32/glew32.dll" $<TARGET_FILE_DIR:standalone>)
	endif(ARCH_X64)

	target_link_libraries(standalone polycore polyengine testgame OpenGL GLEW)

else(WIN32)
	add_custom_command(TARGET standalone POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink "${PROJECT_SOURCE_DIR}/WindowsGLStandalone/Res" $<TARGET_FILE_DIR:standalone>/Res)
	target_link_libraries(standalone polycore polyengine testgame OpenGL GLEW X11 GLX)
endif(WIN32)

cotire(standalone)

#install(TARGETS standalone RUNTIME DESTINATION ".")
