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
target_link_libraries(sgjgame PRIVATE polyengine polycore)
add_dependencies(sgjgame polyrun polyeditor polyrenderingdevicegl)

#note(vuko): we can't use configure_file() directly because there is no reliable way to get an output artifact location at configure-time
find_file(CMAKE_TEMPLATE_PROCESSOR_SCRIPT process_template.cmake  PATHS ${CMAKE_MODULE_PATH}  NO_DEFAULT_PATH)

# Create config file for out game
add_custom_command(
	TARGET sgjgame
	POST_BUILD
	COMMAND "${CMAKE_COMMAND}"
	        -DINPUT_FILE="$<TARGET_PROPERTY:polyengine,SOURCE_DIR>/AssetsPathConfig.json.in"
	        -DOUTPUT_FILE="$<TARGET_FILE_DIR:sgjgame>/AssetsPathConfig.json"
	        -DENGINE_ASSETS_PATHS="$<TARGET_PROPERTY:polyengine,SOURCE_DIR>/Res/"
			-DGAME_ASSETS_PATHS="$<TARGET_PROPERTY:sgjgame,SOURCE_DIR>/Res/"
			-DRENDERING_DEVICE_LIB_PATH="$<TARGET_FILE_NAME:polyrenderingdevicegl>"
			-DGAME_LIB_PATH="$<TARGET_FILE_NAME:sgjgame>"
	        -P "${CMAKE_TEMPLATE_PROCESSOR_SCRIPT}"
)

add_custom_command(
		TARGET sgjgame
		POST_BUILD
			# Inhouse DLL's
			COMMAND ${CMAKE_COMMAND} -E copy  "$<TARGET_FILE:polycore>" "$<TARGET_FILE_DIR:sgjgame>"
			COMMAND ${CMAKE_COMMAND} -E copy  "$<TARGET_FILE:polyengine>" "$<TARGET_FILE_DIR:sgjgame>"
			COMMAND ${CMAKE_COMMAND} -E copy  "$<TARGET_FILE:polyrun>" "$<TARGET_FILE_DIR:sgjgame>"
			COMMAND ${CMAKE_COMMAND} -E copy  "$<TARGET_FILE:polyeditor>" "$<TARGET_FILE_DIR:sgjgame>"
			COMMAND ${CMAKE_COMMAND} -E copy  "$<TARGET_FILE:polyrenderingdevicegl>" "$<TARGET_FILE_DIR:sgjgame>"

			COMMENT "Copying Libs..." VERBATIM
		)

if(WIN32)
	# Copy all necessary DLL's on Windows
	add_custom_command(
		TARGET sgjgame
		POST_BUILD
			# Thirdparty DLL's
			COMMAND ${CMAKE_COMMAND} -E copy  "${CMAKE_SOURCE_DIR}/ThirdParty/SDL2/lib/x64/SDL2.dll" "$<TARGET_FILE_DIR:sgjgame>"
			COMMAND ${CMAKE_COMMAND} -E copy  "${CMAKE_SOURCE_DIR}/ThirdParty/Assimp/lib/Release/x64/assimp-vc140-mt.dll" "$<TARGET_FILE_DIR:sgjgame>"
			COMMAND ${CMAKE_COMMAND} -E copy  "${CMAKE_SOURCE_DIR}/ThirdParty/libepoxy/bin/x64/epoxy-0.dll" "$<TARGET_FILE_DIR:sgjgame>"

			COMMENT "Copying DLLs..." VERBATIM
		)
endif()
cotire(sgjgame)