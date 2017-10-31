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

#note(vuko): we can't use configure_file() directly because there is no reliable way to get an output artifact location at configure-time
find_file(CMAKE_TEMPLATE_PROCESSOR_SCRIPT process_template.cmake  PATHS ${CMAKE_MODULE_PATH}  NO_DEFAULT_PATH)
add_custom_command(
	TARGET sgjgame
	POST_BUILD
	COMMAND "${CMAKE_COMMAND}"
	        -DINPUT_FILE="$<TARGET_PROPERTY:polyrun,SOURCE_DIR>/run_script.in"
	        -DOUTPUT_FILE="$<TARGET_FILE_DIR:sgjgame>/sgjgame"
	        -DRENDERING_DEVICE_LIB_DIR="$<TARGET_FILE_DIR:polyrenderingdevicegl>"
	        -DGAME_LIB_DIR="$<TARGET_FILE_DIR:sgjgame>"
	        -DPOLYRUN_PATH="$<TARGET_FILE:polyrun>"
	        -P "${CMAKE_TEMPLATE_PROCESSOR_SCRIPT}"
)
add_custom_command(
	TARGET sgjgame
	POST_BUILD
	COMMAND "${CMAKE_COMMAND}"
	        -DINPUT_FILE="$<TARGET_PROPERTY:polyengine,SOURCE_DIR>/AssetsPath.json.in"
	        -DOUTPUT_FILE="$<TARGET_FILE_DIR:sgjgame>/AssetsPath.json"
	        -DENGINE_ASSETS_PATHS="$<TARGET_PROPERTY:polyengine,SOURCE_DIR>/Res/"
	        -DGAME_ASSETS_PATHS="$<TARGET_PROPERTY:sgjgame,SOURCE_DIR>/Res/"
	        -P "${CMAKE_TEMPLATE_PROCESSOR_SCRIPT}"
)

cotire(sgjgame)
