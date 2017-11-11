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

find_library(Cocoa_LIBRARY Cocoa)
mark_as_advanced(Cocoa_LIBRARY)

add_executable(polyrun ${STANDALONE_SRCS} ${STANDALONE_H_FOR_IDE})
target_include_directories(polyrun PUBLIC ${STANDALONE_INCLUDE})
target_link_libraries(polyrun
		polyengine
		epoxy::gl
		${Cocoa_LIBRARY}
		)
add_dependencies(polyrun polyrenderingdevicegl) #dynamically loaded

cotire(polyrun)