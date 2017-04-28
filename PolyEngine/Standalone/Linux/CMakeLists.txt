find_package(OpenGL REQUIRED)
if(WIN32)
	find_package(GLEW REQUIRED PATHS ThirdParty/glew/lib/Release/ PATH_SUFFIXES Win32 x64) #FIXME(vuko): see Engine/CMakeLists.txt
else(WIN32)
	find_package(GLEW REQUIRED)
	find_package(X11 REQUIRED)
endif(WIN32)


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
	target_include_directories(standalone BEFORE PRIVATE ThirdParty/glew/include)

	if(WIN32_X64)
		add_custom_command(TARGET standalone POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/ThirdParty/glew/bin/Release/x64/glew32.dll" $<TARGET_FILE_DIR:standalone>)
	else(WIN32_X64)
		add_custom_command(TARGET standalone POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/ThirdParty/glew/bin/Release/Win32/glew32.dll" $<TARGET_FILE_DIR:standalone>)
	endif(WIN32_X64)

	target_link_libraries(standalone polycore polyengine testgame GL GLEW)

else(WIN32)
	add_custom_command(TARGET standalone POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink "${PROJECT_SOURCE_DIR}/WindowsGLStandalone/Res" $<TARGET_FILE_DIR:standalone>/Res)
	target_link_libraries(standalone polycore polyengine testgame GL GLEW X11)
endif(WIN32)

