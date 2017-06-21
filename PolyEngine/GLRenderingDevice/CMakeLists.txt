find_package(OpenGL REQUIRED)
if(WIN32)
	list(APPEND CMAKE_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/ThirdParty/glew/include")
	list(APPEND CMAKE_LIBRARY_PATH "${CMAKE_SOURCE_DIR}/ThirdParty/glew/lib/Release/x64/" "${CMAKE_SOURCE_DIR}/ThirdParty/glew/lib/Release/Win32/")
endif()
find_package(GLEW REQUIRED)

set(POLYGLDEVICE_SRCS
	Src/GLMeshDeviceProxy.cpp
	Src/GLRenderingDevice.cpp
	Src/GLTextFieldBufferDeviceProxy.cpp
	Src/GLTextureDeviceProxy.cpp
	Src/GLWorldRendering.cpp
	Src/GLShaderProgram.cpp
)
set(POLYGLDEVICE_INCLUDE Src)
set(POLYGLDEVICE_H_FOR_IDE
	Src/GLMeshDeviceProxy.hpp
	Src/GLRenderingDevice.hpp
	Src/GLTextFieldBufferDeviceProxy.hpp
	Src/GLTextureDeviceProxy.hpp
	Src/GLUtils.hpp
	Src/GLShaderProgram.hpp
)

add_library(polygldevice SHARED ${POLYGLDEVICE_SRCS} ${POLYGLDEVICE_H_FOR_IDE})
target_compile_definitions(polygldevice PRIVATE _DEVICE)
target_include_directories(polygldevice INTERFACE ${POLYGLDEVICE_INCLUDE} PRIVATE ${OPENGL_INCLUDE_DIR})
target_link_libraries(polygldevice PRIVATE polycore polyengine ${OPENGL_LIBRARIES} GLEW::GLEW )

if(GENERATE_COVERAGE AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
	target_compile_options(polygldevice PRIVATE --coverage -fprofile-arcs -ftest-coverage)
	target_link_libraries(polygldevice PRIVATE -fprofile-arcs -ftest-coverage)
	add_custom_target(coverage_gldevice
	                  COMMAND ${GCOV_COMMAND} -r -o "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/polygldevice.dir/Src" -s ${CMAKE_CURRENT_SOURCE_DIR} ${POLYGLDEVICE_SRCS}
	                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	                  COMMENT "Collecting gcov coverage information for polygldevice"
	)
endif(GENERATE_COVERAGE AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))

#set_target_properties(polygldevice PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "Src/EnginePCH.hpp")
cotire(polygldevice)
