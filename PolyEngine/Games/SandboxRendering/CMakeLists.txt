set(SANDBOX_RENDERING_SRCS
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
set(SANDBOX_RENDERING_INCLUDE Src)
set(SANDBOX_RENDERING_H_FOR_IDE
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

add_library(sandbox_rendering SHARED ${SANDBOX_RENDERING_SRCS} ${SANDBOX_RENDERING_H_FOR_IDE})
target_compile_definitions(sandbox_rendering PRIVATE _GAME)
target_include_directories(sandbox_rendering PRIVATE ${SANDBOX_RENDERING_INCLUDE})
target_link_libraries(sandbox_rendering PRIVATE polyengine)
set_target_properties(sandbox_rendering PROPERTIES OUTPUT_NAME game)

cotire(sandbox_rendering)
