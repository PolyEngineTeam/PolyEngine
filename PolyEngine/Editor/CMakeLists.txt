find_package(SDL2 REQUIRED)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOUIC ON)
SET(AUTOUIC_SEARCH_PATHS Form)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

set(EDITOR_INCLUDE Src)

file(GLOB_RECURSE EDITOR_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
    ${EDITOR_INCLUDE}/*.cpp 
    ${EDITOR_INCLUDE}/*.hpp 
    ${EDITOR_INCLUDE}/*.h)
GenerateSourceGoups("${EDITOR_SRCS}")

add_executable(PolyEditor ${EDITOR_SRCS} ${AUTOGEN_SOURCE_GROUP})
target_include_directories(PolyEditor PUBLIC ${EDITOR_INCLUDE} ${SDL2_INCLUDE_DIRS})
target_link_libraries(PolyEditor PolyEngine PolyCore ${SDL2_LIBRARIES} ${CMAKE_DL_LIBS})
target_link_libraries(PolyEditor Qt5::Widgets Qt5::Core)
add_dependencies(PolyEditor PolyRenderingDeviceGL) #dynamically loaded
qt5_use_modules(PolyEditor Widgets Core)

if(APPLE)
    set(EDITOR_INCLUDE_FOR_APPLE Src-macOS)

    file(GLOB_RECURSE EDITOR_SRCS_FOR_APPLE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
        ${EDITOR_INCLUDE_FOR_APPLE}/*.mm
        ${EDITOR_INCLUDE_FOR_APPLE}/*.cpp
        ${EDITOR_INCLUDE_FOR_APPLE}/*.hpp
        ${EDITOR_INCLUDE_FOR_APPLE}/*.h)
    GenerateSourceGoups("${EDITOR_SRCS_FOR_APPLE}")

    find_library(Cocoa_LIBRARY Cocoa)
    mark_as_advanced(Cocoa_LIBRARY)

    target_sources(PolyEditor PRIVATE ${EDITOR_SRCS_FOR_APPLE})
    target_include_directories(PolyEditor PRIVATE ${EDITOR_INCLUDE_FOR_APPLE})
    target_link_libraries(PolyEditor ${Cocoa_LIBRARY})
endif()

add_custom_command(
		TARGET PolyEditor
		POST_BUILD
			# Inhouse shared library dependancies
			COMMAND ${CMAKE_COMMAND} -E copy  "$<TARGET_FILE:PolyCore>" "$<TARGET_FILE_DIR:PolyEditor>"
			COMMAND ${CMAKE_COMMAND} -E copy  "$<TARGET_FILE:PolyEngine>" "$<TARGET_FILE_DIR:PolyEditor>"
			COMMAND ${CMAKE_COMMAND} -E copy  "$<TARGET_FILE:PolyRenderingDeviceGL>" "$<TARGET_FILE_DIR:PolyEditor>"

			COMMENT "Copying Libs..." VERBATIM
		)

if(WIN32)
	# Copy all necessary DLL's on Windows
	add_custom_command(
		TARGET PolyEditor
		POST_BUILD
			# Thirdparty DLL's
			COMMAND ${CMAKE_COMMAND} -E copy  "${ENGINE_ROOT_DIR}/ThirdParty/SDL2/lib/x64/SDL2.dll" "$<TARGET_FILE_DIR:PolyEditor>"
			COMMAND ${CMAKE_COMMAND} -E copy  "${ENGINE_ROOT_DIR}/ThirdParty/Assimp/lib/Release/x64/assimp-vc140-mt.dll" "$<TARGET_FILE_DIR:PolyEditor>"
			COMMAND ${CMAKE_COMMAND} -E copy  "${ENGINE_ROOT_DIR}/ThirdParty/libepoxy/bin/x64/epoxy-0.dll" "$<TARGET_FILE_DIR:PolyEditor>"

			COMMENT "Copying DLLs..." VERBATIM
		)
endif()

# Strange bug in cotire for macOS. Disable it for now. Include paths are not working.
if(NOT APPLE)
    cotire(PolyEditor)
endif()
