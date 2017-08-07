set(STANDALONE_SRCS Src/Main.cpp)

add_executable(standalone ${STANDALONE_SRCS})
set_target_properties(standalone PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
target_link_libraries(standalone polycore polyengine polygldevice sgjgame)
add_custom_command(
		TARGET standalone POST_BUILD
		COMMAND
		${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/ThirdParty/libepoxy/bin/$<${ARCH_X64}:x64>$<${ARCH_X86}:Win32>/epoxy-0.dll"
		                                      "$<TARGET_FILE_DIR:standalone>"
		COMMAND
		${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/ThirdParty/assimp/lib/Release/$<${ARCH_X64}:x64>$<${ARCH_X86}:Win32>/assimp-vc140-mt.dll"
		                                      "$<TARGET_FILE_DIR:standalone>"
)

cotire(standalone)
