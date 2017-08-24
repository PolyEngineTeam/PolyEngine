set(CZOLG_INVADERS_SRCS
	Src/InvadersGame.cpp
	Src/BulletComponent.cpp
	Src/CollisionSystem.cpp
	Src/ControlSystem.cpp
	Src/EnemyMovementComponent.cpp
	Src/GameManagerComponent.cpp
	Src/GameManagerSystem.cpp
	Src/MovementSystem.cpp
	Src/PlayerControllerComponent.cpp
)
set(CZOLG_INVADERS_INCLUDE Src)
set(CZOLG_INVADERS_H_FOR_IDE
	Src/InvadersGame.hpp
	Src/BulletComponent.hpp
	Src/CollisionSystem.hpp
	Src/CollisionComponent.hpp
	Src/ControlSystem.hpp
	Src/EnemyMovementComponent.hpp
	Src/GameManagerComponent.hpp
	Src/MovementComponent.hpp
	Src/GameManagerSystem.hpp
	Src/MovementSystem.hpp
	Src/PlayerControllerComponent.hpp
)

add_library(czolg_invaders SHARED ${CZOLG_INVADERS_SRCS} ${CZOLG_INVADERS_H_FOR_IDE})
target_compile_definitions(czolg_invaders PRIVATE _GAME)
target_include_directories(czolg_invaders PRIVATE ${CZOLG_INVADERS_INCLUDE})
target_link_libraries(czolg_invaders PRIVATE polyengine)
set_target_properties(czolg_invaders PROPERTIES OUTPUT_NAME game)

cotire(czolg_invaders)
