find_package(SDL2 REQUIRED)
find_package(epoxy REQUIRED)

set(POLYGLDEVICE_SRCS
	Src/UnlitRenderingPass.cpp
	Src/BlinnPhongRenderingPass.cpp
	Src/DebugNormalsRenderingPass.cpp
	Src/DebugNormalsWireframeRenderingPass.cpp
	Src/GLMeshDeviceProxy.cpp
	Src/GLCubemapDeviceProxy.cpp
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
set(POLYGLDEVICE_INCLUDE Src)
set(POLYGLDEVICE_H_FOR_IDE
	Src/UnlitRenderingPass.hpp
	Src/BlinnPhongRenderingPass.hpp
	Src/DebugNormalsRenderingPass.hpp
	Src/DebugNormalsWireframeRenderingPass.hpp
	Src/GLMeshDeviceProxy.hpp
	Src/GLCubemapDeviceProxy.hpp
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

add_library(polyrenderingdevicegl SHARED ${POLYGLDEVICE_SRCS} ${POLYGLDEVICE_H_FOR_IDE})
target_compile_definitions(polyrenderingdevicegl PRIVATE _DEVICE)
target_include_directories(polyrenderingdevicegl PUBLIC ${POLYGLDEVICE_INCLUDE} ${SDL2_INCLUDE_DIRS})
target_link_libraries(polyrenderingdevicegl PRIVATE polyengine epoxy::gl ${SDL2_LIBRARIES})
set_target_properties(polyrenderingdevicegl PROPERTIES OUTPUT_NAME polyrenderingdevice)

cotire(polyrenderingdevicegl)
