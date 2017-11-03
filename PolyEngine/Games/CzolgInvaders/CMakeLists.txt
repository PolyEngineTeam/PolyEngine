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

#note(vuko): we can't use configure_file() directly because there is no reliable way to get an output artifact location at configure-time
find_file(CMAKE_TEMPLATE_PROCESSOR_SCRIPT process_template.cmake  PATHS ${CMAKE_MODULE_PATH}  NO_DEFAULT_PATH)
add_custom_command(
	TARGET czolg_invaders
	POST_BUILD
	COMMAND "${CMAKE_COMMAND}"
	        -DINPUT_FILE="$<TARGET_PROPERTY:polyrun,SOURCE_DIR>/run_script.in"
	        -DOUTPUT_FILE="$<TARGET_FILE_DIR:czolg_invaders>/czolg_invaders"
	        -DRENDERING_DEVICE_LIB_DIR="$<TARGET_FILE_DIR:polyrenderingdevicegl>"
	        -DGAME_LIB_DIR="$<TARGET_FILE_DIR:czolg_invaders>"
	        -DPOLYRUN_PATH="$<TARGET_FILE:polyrun>"
	        -P "${CMAKE_TEMPLATE_PROCESSOR_SCRIPT}"
)
add_custom_command(
	TARGET czolg_invaders
	POST_BUILD
	COMMAND "${CMAKE_COMMAND}"
	        -DINPUT_FILE="$<TARGET_PROPERTY:polyengine,SOURCE_DIR>/AssetsPath.json.in"
	        -DOUTPUT_FILE="$<TARGET_FILE_DIR:czolg_invaders>/AssetsPath.json"
	        -DENGINE_ASSETS_PATHS="$<TARGET_PROPERTY:polyengine,SOURCE_DIR>/Res/"
	        -DGAME_ASSETS_PATHS="$<TARGET_PROPERTY:czolg_invaders,SOURCE_DIR>/Res/"
	        -P "${CMAKE_TEMPLATE_PROCESSOR_SCRIPT}"
)

cotire(czolg_invaders)
