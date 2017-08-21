find_package(epoxy REQUIRED)

# TODO Are these necessary?
#set(CMAKE_C_FLAGS "-x objective-c")
#set(CMAKE_CXX_FLAGS "-x objective-c++")

set(STANDALONE_SRCS 
	Src/main.m
	Src/ViewController.m
	Src/OpenGLView.m
	Src/AppDelegate.m
)

set(STANDALONE_INCLUDE Src)
set(STANDALONE_H_FOR_IDE
	Src/ViewController.h
	Src/OpenGLView.h
	Src/AppDelegate.h
)


add_executable(standalone ${STANDALONE_SRCS})
# TODO proper linking with engine

#target_link_libraries(standalone polycore polyengine polygldevice sgjgame ${X11_LIBRARIES} epoxy::gl)
target_link_libraries(standalone
    "-framework Cocoa"
    #”-framework AppKit"
    #”-framework CoreData"
    #”-framework Foundation"
    "-framework OpenGL"
)

cotire(standalone)
