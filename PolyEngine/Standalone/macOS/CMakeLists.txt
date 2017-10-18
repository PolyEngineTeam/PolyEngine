find_package(epoxy REQUIRED)

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

find_library(Cocoa_LIBRARY Cocoa)
mark_as_advanced(Cocoa_LIBRARY)

target_link_libraries(standalone polycore polyengine polygldevice sgjgame
		${Cocoa_LIBRARY} epoxy::gl)

cotire(standalone)
