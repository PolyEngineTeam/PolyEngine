cmake_minimum_required(VERSION 3.2)
project(PolyEngine VERSION 0.1.2 LANGUAGES CXX C) #note(vuko): without C older CMake versions fail to process FindX11 the first time

##
# Initial setup: C++ standard version, build types, CMake modules
##
set(ENGINE_ROOT_DIR ${CMAKE_SOURCE_DIR})
set(DIST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Dist")

include(${ENGINE_ROOT_DIR}/CMakeListsCommon.cmake)

##
# Finally, meat and potatoes
##
add_subdirectory(Core)
add_subdirectory(Engine)
add_subdirectory(EngineAPI)
add_subdirectory(RenderingDevice/OpenGL)
add_subdirectory(Editor)
add_subdirectory(Standalone)
enable_testing()
add_subdirectory(UnitTests)

##
# Doxygen
##
if(GENERATE_DOXYGEN)
	find_package(Doxygen)
	if(DOXYGEN_FOUND AND false) #TODO(vuko): reenable after Doxyfile.in gets written
		set(DOC_DIR ${PROJECT_BINARY_DIR}/doc)
		configure_file(${PROJECT_SOURCE_DIR}/Doxygen/Doxyfile.in ${DOC_DIR}/Doxyfile @ONLY)
		add_custom_target(docs
		                  COMMAND ${DOXYGEN_EXECUTABLE}
		                  WORKING_DIRECTORY ${DOC_DIR}
		                  COMMENT "Building doxygen documentation"
		                  SOURCES ${PROJECT_SOURCE_DIR}/Doxygen/Doxyfile.in
		)
		source_group("" FILES ${PROJECT_SOURCE_DIR}/Docs/Doxyfile.in)
	endif()
endif()