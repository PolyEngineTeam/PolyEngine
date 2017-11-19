find_package(OpenAL REQUIRED)

set(POLYALDEVICE_SRCS
	Src/ALSoundDevice.cpp
	Src/SoundDataHolder.cpp
)
set(POLYALDEVICE_INCLUDE Src)
set(POLYALDEVICE_H_FOR_IDE
	Src/ALSoundDevice.hpp
	Src/SoundDataHolder.hcpp
)

add_library(polysounddeviceal SHARED ${POLYALDEVICE_SRCS} ${POLYALDEVICE_H_FOR_IDE})
target_compile_definitions(polysounddeviceal PRIVATE _SOUND_DEVICE)
target_include_directories(polysounddeviceal PUBLIC ${POLYALDEVICE_INCLUDE})
target_link_libraries(polysounddeviceal PRIVATE polyengine OpenAL::AL)
set_target_properties(polysounddeviceal PROPERTIES OUTPUT_NAME polysounddevice)

cotire(polysounddeviceal)
