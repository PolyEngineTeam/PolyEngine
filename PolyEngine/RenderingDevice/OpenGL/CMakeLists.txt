find_package(epoxy REQUIRED)

set(POLYGLDEVICE_SRCS
	Src/UnlitRenderingPass.cpp
	Src/BlinnPhongRenderingPass.cpp
	Src/DebugNormalsRenderingPass.cpp
	Src/GLMeshDeviceProxy.cpp
	Src/GLRenderingDevice.cpp
	Src/GLShaderProgram.cpp
	Src/GLTextFieldBufferDeviceProxy.cpp
	Src/GLTextureDeviceProxy.cpp
	Src/GLWorldRendering.cpp
	Src/PostprocessRenderingPass.cpp
	Src/PostprocessQuad.cpp
	Src/RenderingPassBase.cpp
	Src/Text2DRenderingPass.cpp
	Src/TransparentRenderingPass.cpp
)
set(POLYGLDEVICE_SRCS_APPLE
	Src-macOS/CocoaImpl.mm
)
set(POLYGLDEVICE_INCLUDE Src)
set(POLYGLDEVICE_INCLUDE_APPLE Src-macOS)
set(POLYGLDEVICE_H_FOR_IDE
	Src/UnlitRenderingPass.hpp
	Src/BlinnPhongRenderingPass.hpp
	Src/DebugNormalsRenderingPass.hpp
	Src/GLMeshDeviceProxy.hpp
	Src/GLRenderingDevice.hpp
	Src/GLShaderProgram.hpp
	Src/GLTextFieldBufferDeviceProxy.hpp
	Src/GLTextureDeviceProxy.hpp
	Src/GLUtils.hpp
	Src/PostprocessRenderingPass.hpp
	Src/PostprocessQuad.hpp
	Src/RenderingPassBase.hpp
	Src/Text2DRenderingPass.hpp
	Src/TransparentRenderingPass.hpp
)
set(POLYGLDEVICE_H_FOR_IDE_APPLE
	Src-macOS/CocoaImpl.h
	Src-macOS/ObjCInterface.hpp
)

add_library(polyrenderingdevicegl SHARED ${POLYGLDEVICE_SRCS} ${POLYGLDEVICE_H_FOR_IDE})
target_compile_definitions(polyrenderingdevicegl PRIVATE _DEVICE)
target_include_directories(polyrenderingdevicegl PUBLIC ${POLYGLDEVICE_INCLUDE})
target_link_libraries(polyrenderingdevicegl PRIVATE polyengine epoxy::gl)
set_target_properties(polyrenderingdevicegl PROPERTIES OUTPUT_NAME polyrenderingdevice)

if(APPLE)
	find_library(Cocoa_LIBRARY Cocoa)
	mark_as_advanced(Cocoa_LIBRARY)

	target_sources(polyrenderingdevicegl PRIVATE ${POLYGLDEVICE_SRCS_APPLE} ${POLYGLDEVICE_H_FOR_IDE_APPLE})
	target_include_directories(polyrenderingdevicegl PUBLIC ${POLYGLDEVICE_INCLUDE_APPLE})
	target_link_libraries(polyrenderingdevicegl PRIVATE ${Cocoa_LIBRARY})
endif()

cotire(polyrenderingdevicegl)
