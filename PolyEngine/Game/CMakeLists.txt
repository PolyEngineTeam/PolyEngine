set(TESTGAME_SRCS Src/TestGame.cpp)
set(TESTGAME_INCLUDE Src)
set(TESTGAME_H_FOR_IDE Src/TestGame.hpp)

add_library(testgame SHARED ${TESTGAME_SRCS} ${TESTGAME_H_FOR_IDE})
target_compile_definitions(testgame PRIVATE _GAME)
target_include_directories(testgame INTERFACE ${TESTGAME_INCLUDE})
target_link_libraries(testgame polycore polyengine)

cotire(testgame)
