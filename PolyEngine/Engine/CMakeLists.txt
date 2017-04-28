find_package(OpenGL REQUIRED)
if(WIN32)
	find_package(GLEW REQUIRED PATHS ThirdParty/glew/lib/Release/ PATH_SUFFIXES Win32 x64) #FIXME(vuko): does this even work? how to make it search default paths first? (according to docs it should by default, but it fails on my system as opposed to invocation without given paths)
else(WIN32)
	find_package(GLEW REQUIRED)
endif(WIN32)


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
	target_include_directories(polyengine BEFORE PRIVATE ThirdParty/glew/include)
	if(WIN32_X64)
		 add_custom_command(TARGET polyengine POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/ThirdParty/glew/bin/Release/x64/glew32.dll" $<TARGET_FILE_DIR:polyengine>)
	else(WIN32_X64)
		add_custom_command(TARGET polyengine POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/ThirdParty/glew/bin/Release/Win32/glew32.dll" $<TARGET_FILE_DIR:polyengine>)
	endif(WIN32_X64)
endif(WIN32)
target_link_libraries(polyengine polycore OpenGL GLEW)

