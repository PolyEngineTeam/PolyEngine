find_package(epoxy REQUIRED)

set(POLYGLDEVICE_SRCS
	Src/BlinnPhongRenderingPass.cpp
	Src/DebugNormalsRenderingPass.cpp
	Src/GLMeshDeviceProxy.cpp
	Src/GLRenderingDevice.cpp
	Src/GLShaderProgram.cpp
	Src/GLTextFieldBufferDeviceProxy.cpp
	Src/GLTextureDeviceProxy.cpp
	Src/GLWorldRendering.cpp
	Src/PostprocessRenderingPass.cpp
	Src/RenderingPassBase.cpp
	Src/Text2DRenderingPass.cpp
	Src/TransparentRenderingPass.cpp
)
set(POLYGLDEVICE_INCLUDE Src)
set(POLYGLDEVICE_H_FOR_IDE
	Src/BlinnPhongRenderingPass.hpp
	Src/DebugNormalsRenderingPass.hpp
	Src/GLMeshDeviceProxy.hpp
	Src/GLRenderingDevice.hpp
	Src/GLShaderProgram.hpp
	Src/GLTextFieldBufferDeviceProxy.hpp
	Src/GLTextureDeviceProxy.hpp
	Src/GLUtils.hpp
	Src/PostprocessRenderingPass.hpp
	Src/RenderingPassBase.hpp
	Src/Text2DRenderingPass.hpp
	Src/TransparentRenderingPass.hpp
)

add_library(polygldevice SHARED ${POLYGLDEVICE_SRCS} ${POLYGLDEVICE_H_FOR_IDE})
target_compile_definitions(polygldevice PRIVATE _DEVICE)
target_include_directories(polygldevice INTERFACE ${POLYGLDEVICE_INCLUDE})
target_link_libraries(polygldevice PRIVATE polycore polyengine epoxy::gl)

cotire(polygldevice)
