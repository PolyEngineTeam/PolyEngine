set(TESTGAME_SRCS 
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
set(TESTGAME_INCLUDE Src)
set(TESTGAME_H_FOR_IDE 
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

add_library(testgame SHARED ${TESTGAME_SRCS} ${TESTGAME_H_FOR_IDE})
target_compile_definitions(testgame PRIVATE _GAME)
target_include_directories(testgame INTERFACE ${TESTGAME_INCLUDE})
target_link_libraries(testgame polycore polyengine)

cotire(testgame)
