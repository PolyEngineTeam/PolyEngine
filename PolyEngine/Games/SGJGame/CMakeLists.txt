set(SGJGAME_SRCS
	Src/CameraMovementSystem.cpp
	Src/Game.cpp
	Src/GameDebugSystem.cpp
	Src/GameDebugWorldComponent.cpp
	Src/GameManagerSystem.cpp
	Src/GameManagerWorldComponent.cpp
	Src/Level.cpp
	Src/PlayerControllerComponent.cpp
	Src/PlayerUpdateSystem.cpp
	Src/PowerupSystem.cpp
	Src/TileComponent.cpp
)
set(SGJGAME_INCLUDE Src)
set(SGJGAME_H_FOR_IDE
	Src/CameraMovementComponent.hpp
	Src/CameraMovementSystem.hpp
	Src/Game.hpp
	Src/GameDebugSystem.hpp
	Src/GameDebugWorldComponent.hpp
	Src/GameManagerSystem.hpp
	Src/GameManagerWorldComponent.hpp
	Src/Level.hpp
	Src/PlayerControllerComponent.hpp
	Src/PlayerUpdateSystem.hpp
	Src/PowerupSystem.hpp
	Src/Powerups.hpp
	Src/TileComponent.hpp
)

add_library(sgjgame SHARED ${SGJGAME_SRCS} ${SGJGAME_H_FOR_IDE})
target_compile_definitions(sgjgame PRIVATE _GAME)
target_include_directories(sgjgame PRIVATE ${SGJGAME_INCLUDE})
target_link_libraries(sgjgame PRIVATE polyengine)
set_target_properties(sgjgame PROPERTIES OUTPUT_NAME game)

cotire(sgjgame)
