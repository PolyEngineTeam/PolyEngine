find_package(SOIL REQUIRED)
find_package(assimp REQUIRED)
find_package(Freetype REQUIRED)
find_package(OpenAL REQUIRED)
find_package(OGG REQUIRED)
find_package(Vorbis REQUIRED)
find_package(Box2D REQUIRED)
find_package(RapidJSON REQUIRED)

set(POLYENGINE_SRCS
	Src/CameraComponent.cpp
	Src/CameraSystem.cpp
	Src/CoreConfig.cpp
	Src/DebugWorldComponent.cpp
	Src/DeferredTaskSystem.cpp
	Src/Engine.cpp
	Src/Entity.cpp
	Src/FontResource.cpp
	Src/FPSSystem.cpp
	Src/FreeFloatMovementComponent.cpp
	Src/InputSystem.cpp
	Src/InputWorldComponent.cpp
	Src/LightSourceComponent.cpp
	Src/Mesh.cpp
	Src/MeshRenderingComponent.cpp
	Src/MeshResource.cpp
	Src/MovementSystem.cpp
	Src/OpenALDevice.cpp
	Src/Physics2DColliders.cpp
	Src/Physics2DSystem.cpp
	Src/Physics2DWorldComponent.cpp
	Src/PostprocessSettingsComponent.cpp
	Src/RenderingSystem.cpp
	Src/ResourceManager.cpp
	Src/Rigidbody2DComponent.cpp
	Src/SoundEmitterComponent.cpp
	Src/SoundListenerComponent.cpp
	Src/SoundResource.cpp
	Src/SoundSystem.cpp
	Src/SoundWorldComponent.cpp
	Src/Text2D.cpp
	Src/TextureResource.cpp
	Src/TimeSystem.cpp
	Src/TimeWorldComponent.cpp
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
	Src/DebugWorldComponent.hpp
	Src/DeferredTaskBase.hpp
	Src/DeferredTaskImplementation.hpp
	Src/DeferredTaskSystem.hpp
	Src/DeferredTaskWorldComponent.hpp
	Src/Engine.hpp
	Src/EnginePCH.hpp
	Src/Entity.hpp
	Src/FontResource.hpp
	Src/FPSSystem.hpp
	Src/FreeFloatMovementComponent.hpp
	Src/InputQueue.hpp
	Src/InputSystem.hpp
	Src/InputWorldComponent.hpp
	Src/IRenderingDevice.hpp
	Src/KeyBindings.hpp
	Src/LightSourceComponent.hpp
	Src/Mesh.hpp
	Src/MeshRenderingComponent.hpp
	Src/MeshResource.hpp
	Src/MovementSystem.hpp
	Src/OpenALDevice.hpp
	Src/Physics2DColliders.hpp
	Src/Physics2DSystem.hpp
	Src/Physics2DWorldComponent.hpp
	Src/PostprocessSettingsComponent.hpp
	Src/RenderingSystem.hpp
	Src/ResourceBase.hpp
	Src/ResourceManager.hpp
	Src/Rigidbody2DComponent.hpp
	Src/RigidBody2DImpl.hpp
	Src/ScreenSpaceTextComponent.hpp
	Src/SoundEmitterComponent.hpp
	Src/SoundListenerComponent.hpp
	Src/SoundResource.hpp
	Src/SoundSystem.hpp
	Src/SoundWorldComponent.hpp
	Src/Text2D.hpp
	Src/TextureResource.hpp
	Src/Timer.hpp
	Src/TimeSystem.hpp
	Src/TimeWorldComponent.hpp
	Src/TransformComponent.hpp
	Src/Viewport.hpp
	Src/ViewportWorldComponent.hpp
	Src/World.hpp
)

add_library(polyengine SHARED ${POLYENGINE_SRCS} ${POLYENGINE_H_FOR_IDE})
target_compile_definitions(polyengine PRIVATE _ENGINE)
target_include_directories(polyengine INTERFACE ${POLYENGINE_INCLUDE})
target_link_libraries(polyengine PRIVATE polycore SOIL::SOIL ass::imp Freetype::FT2 OpenAL::AL OGG::OGG Vorbis::Vorbis Vorbis::File Box2D::Box2D Rapid::JSON)

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
