find_package(epoxy REQUIRED)

set(POLYALDEVICE_INCLUDE Src)
set(POLYALDEVICE_H_FOR_IDE
	Src/ALSoundDevice.hpp
	Src/SoundDataHolder.hcpp
)
set(POLYALDEVICE_SRCS
	Src/ALSoundDevice.cpp
	Src/SoundDataHolder.cpp
)

set(POLYALDEVICE_INCLUDE_APPLE Src-macOS)
set(POLYALDEVICE_H_FOR_IDE_APPLE
	Src-macOS/CocoaImpl.h
	Src-macOS/ObjCInterface.hpp
)
set(POLYALDEVICE_SRCS_APPLE
	Src-macOS/CocoaImpl.mm
)

add_library(polysounddeviceal SHARED ${POLYALDEVICE_SRCS} ${POLYALDEVICE_H_FOR_IDE})
target_compile_definitions(polysounddeviceal PRIVATE _SOUND_DEVICE)
target_include_directories(polysounddeviceal PUBLIC ${POLYALDEVICE_INCLUDE})
target_link_libraries(polysounddeviceal PRIVATE polyengine epoxy::gl)
set_target_properties(polysounddeviceal PROPERTIES OUTPUT_NAME polysounddevice)

if(APPLE)
	target_sources(polysounddeviceal PRIVATE ${POLYALDEVICE_SRCS_APPLE} ${POLYALDEVICE_H_FOR_IDE_APPLE})
	target_include_directories(polysounddeviceal PUBLIC ${POLYALDEVICE_INCLUDE_APPLE})
	target_link_libraries(polysounddeviceal PRIVATE ${Cocoa_LIBRARY})
endif()

cotire(polysounddeviceal)
