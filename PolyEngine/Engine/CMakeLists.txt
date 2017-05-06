find_package(OpenGL REQUIRED)
if(WIN32)
	list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/ThirdParty/glew")
endif(WIN32)
find_package(GLEW REQUIRED)


set(POLYENGINE_SRCS
	Src/Engine.cpp
	Src/Entity.cpp
	Src/OpenGLRenderingContext.cpp
	Src/World.cpp
	Src/CameraComponent.cpp
	Src/CameraSystem.cpp
	Src/InputSystem.cpp
	Src/InputWorldComponent.cpp
	Src/MeshRenderingComponent.cpp
	Src/RenderingSystem.cpp
	Src/ResourceManager.cpp
	Src/ShaderProgram.cpp
	Src/TransformComponent.cpp
	Src/ViewportWorldComponent.cpp
)
set(POLYENGINE_INCLUDE
	Src
)
set(POLYENGINE_H_FOR_IDE
	Src/CameraComponent.hpp
	Src/CameraSystem.hpp
	Src/ComponentBase.hpp
	Src/Engine.hpp
	Src/EnginePCH.hpp
	Src/Entity.hpp
	Src/InputQueue.hpp
	Src/InputSystem.hpp
	Src/InputWorldComponent.hpp
	Src/KeyBindings.hpp
	Src/MeshRenderingComponent.hpp
	Src/OpenGLRenderingContext.hpp
	Src/RenderingContext.hpp
	Src/RenderingSystem.hpp
	Src/ResourceManager.hpp
	Src/ShaderProgram.hpp
	Src/TransformComponent.hpp
	Src/Viewport.hpp
	Src/ViewportWorldComponent.hpp
	Src/World.hpp
)

add_library(polyengine SHARED ${POLYENGINE_SRCS} ${POLYENGINE_H_FOR_IDE})
target_compile_definitions(polyengine PRIVATE _ENGINE)
target_include_directories(polyengine INTERFACE ${POLYENGINE_INCLUDE})
if(WIN32)
	#target_include_directories(polyengine BEFORE PRIVATE "${CMAKE_SOURCE_DIR}/ThirdParty/glew")
	if(ARCH_X64)
		 add_custom_command(TARGET polyengine POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/ThirdParty/glew/bin/Release/x64/glew32.dll" $<TARGET_FILE_DIR:polyengine>)
	else(ARCH_X64)
		add_custom_command(TARGET polyengine POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/ThirdParty/glew/bin/Release/Win32/glew32.dll" $<TARGET_FILE_DIR:polyengine>)
	endif(ARCH_X64)
endif(WIN32)
target_link_libraries(polyengine PRIVATE polycore OpenGL GLEW)
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	target_link_libraries(polyengine PRIVATE X11 GLX) #some linkers may need this (ld does just fine without it and gold unfortunately chokes)
endif()
if(GENERATE_COVERAGE AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
	target_compile_options(polyengine PRIVATE --coverage -fprofile-arcs -ftest-coverage)
	target_link_libraries(polyengine PRIVATE -fprofile-arcs -ftest-coverage)
	file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/gcov)
	add_custom_target(coverage_engine
	                  COMMAND ${GCOV_COMMAND} -r -o "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/polyengine.dir/Src" -s ${CMAKE_CURRENT_SOURCE_DIR} ${POLYENGINE_SRCS}
	                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/gcov
	                  COMMENT "Collecting gcov coverage information for polyengine"
	)
endif(GENERATE_COVERAGE AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))

set_target_properties(polyengine PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "Src/EnginePCH.hpp")
cotire(polyengine)
