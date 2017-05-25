find_package(OpenGL REQUIRED)
if(WIN32)
	list(APPEND CMAKE_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/ThirdParty/glew/include")
	list(APPEND CMAKE_LIBRARY_PATH "${CMAKE_SOURCE_DIR}/ThirdParty/glew/lib/Release/x64/" "${CMAKE_SOURCE_DIR}/ThirdParty/glew/lib/Release/Win32/")
	list(APPEND CMAKE_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/ThirdParty/SOIL/include")
	list(APPEND CMAKE_LIBRARY_PATH "${CMAKE_SOURCE_DIR}/ThirdParty/SOIL/lib/Release/x64/" "${CMAKE_SOURCE_DIR}/ThirdParty/SOIL/lib/Release/Win32/")
	set(ASSIMP_ROOT_DIR "${CMAKE_SOURCE_DIR}/ThirdParty/assimp")
endif()
find_package(GLEW REQUIRED)
find_package(SOIL REQUIRED)
find_package(assimp REQUIRED)

set(POLYENGINE_SRCS
	Src/CameraComponent.cpp
	Src/CameraSystem.cpp
	Src/CoreConfig.cpp
	Src/Engine.cpp
	Src/Entity.cpp
	Src/FreeFloatMovementComponent.cpp
	Src/GLMeshResource.cpp
	Src/GLTextureResource.cpp
	Src/InputSystem.cpp
	Src/InputWorldComponent.cpp
	Src/MeshRenderingComponent.cpp
	Src/MovementSystem.cpp
	Src/OpenGLRenderingContext.cpp
	Src/RenderingSystem.cpp
	Src/ResourceManager.cpp
	Src/ShaderProgram.cpp
	Src/TimeSystem.cpp
	Src/TimeWorldComponent.cpp
	Src/Timer.cpp
	Src/TransformComponent.cpp
	Src/ViewportWorldComponent.cpp
	Src/World.cpp
)
set(POLYENGINE_INCLUDE Src)
set(POLYENGINE_H_FOR_IDE
	Src/CameraComponent.hpp
	Src/CameraSystem.hpp
	Src/ComponentBase.hpp
	Src/CoreConfig.hpp
	Src/Engine.hpp
	Src/EnginePCH.hpp
	Src/Entity.hpp
	Src/FreeFloatMovementComponent.hpp
	Src/GLMeshResource.hpp
	Src/GLTextureResource.hpp
	Src/InputQueue.hpp
	Src/InputSystem.hpp
	Src/InputWorldComponent.hpp
	Src/KeyBindings.hpp
	Src/MeshRenderingComponent.hpp
	Src/MovementSystem.hpp
	Src/OpenGLRenderingContext.hpp
	Src/OpenGLUtils.hpp
	Src/RenderingContext.hpp
	Src/RenderingSystem.hpp
	Src/ResourceBase.hpp
	Src/ResourceManager.hpp
	Src/ShaderProgram.hpp
	Src/TimeSystem.hpp
	Src/TimeWorldComponent.hpp
	Src/Timer.hpp
	Src/TransformComponent.hpp
	Src/Viewport.hpp
	Src/ViewportWorldComponent.hpp
	Src/World.hpp
)

add_library(polyengine SHARED ${POLYENGINE_SRCS} ${POLYENGINE_H_FOR_IDE})
target_compile_definitions(polyengine PRIVATE _ENGINE)
target_include_directories(polyengine INTERFACE ${POLYENGINE_INCLUDE} PRIVATE ${OPENGL_INCLUDE_DIR} ${ASSIMP_INCLUDE_DIRS})
target_link_libraries(polyengine PRIVATE polycore ${OPENGL_LIBRARIES} GLEW::GLEW SOIL::SOIL ${ASSIMP_LIBRARIES})

if(GENERATE_COVERAGE AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
	target_compile_options(polyengine PRIVATE --coverage -fprofile-arcs -ftest-coverage)
	target_link_libraries(polyengine PRIVATE -fprofile-arcs -ftest-coverage)
	add_custom_target(coverage_engine
	                  COMMAND ${GCOV_COMMAND} -r -o "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/polyengine.dir/Src" -s ${CMAKE_CURRENT_SOURCE_DIR} ${POLYENGINE_SRCS}
	                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	                  COMMENT "Collecting gcov coverage information for polyengine"
	)
endif(GENERATE_COVERAGE AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))

set_target_properties(polyengine PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "Src/EnginePCH.hpp")
cotire(polyengine)
