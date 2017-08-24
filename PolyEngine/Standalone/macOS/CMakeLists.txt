find_package(epoxy REQUIRED)

# TODO Are these necessary?
#set(CMAKE_C_FLAGS "-x objective-c")
#set(CMAKE_CXX_FLAGS "-x objective-c++")

set(STANDALONE_SRCS 
	Src/main.mm
	Src/AppDelegate.mm
	Src/Window.mm
	Src/CppInterface.cpp
)

set(STANDALONE_INCLUDE Src)
set(STANDALONE_H_FOR_IDE
	Src/AppDelegate.h
	Src/Window.h
	Src/CppInterface.hpp
)


add_executable(standalone ${STANDALONE_SRCS})
# TODO proper linking with engine

target_link_libraries(standalone polycore polyengine polygldevice sgjgame
    "-framework Cocoa"
    #”-framework AppKit"
    #”-framework CoreData"
    #”-framework Foundation"
)

cotire(standalone)
