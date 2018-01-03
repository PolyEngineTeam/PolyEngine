# FindSOIL
# --------
#
# Find the Simple OpenGL Image Library (SOIL)
#
# IMPORTED Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the :prop_tgt:`IMPORTED` target ``SOIL::SOIL``,
# if SOIL has been found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ::
#
#   SOIL_INCLUDE_DIRS - include directories for SOIL
#   SOIL_LIBRARIES - libraries to link against SOIL
#   SOIL_FOUND - true if SOIL has been found and can be used

if(SOIL_FOUND)
	return()
endif()

include(CMakeFindDependencyMacro)
include(FindPackageHandleStandardArgs)

if(NOT CMAKE_VERSION VERSION_GREATER 3.6)
	find_package(OpenGL REQUIRED)
else()
	find_dependency(OpenGL) #note(vuko): for some reason this makes older CMake versions stop processing
endif()

if (WIN32)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(SOIL_ARCH "x64")
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(SOIL_ARCH "Win32")
	endif()

	set(SOIL_ROOT_DIR "${ENGINE_ROOT_DIR}/ThirdParty/SOIL" CACHE PATH "SOIL root directory")
	set(INCLUDE_HINTS "${SOIL_ROOT_DIR}/include")
	set(LIB_HINTS     "${SOIL_ROOT_DIR}/lib/Release/${SOIL_ARCH}")
endif()

if (APPLE)
	find_library(CoreFoundation_LIBRARY CoreFoundation)
	mark_as_advanced(CoreFoundation_LIBRARY)
	set(CoreFoundation_DEPENDENCY_VAR CoreFoundation_LIBRARY)
endif()

find_path(SOIL_INCLUDE_DIR  NAMES SOIL/SOIL.h  HINTS ${INCLUDE_HINTS})
find_library(SOIL_LIBRARY   NAMES SOIL soil    HINTS ${LIB_HINTS})

set(SOIL_INCLUDE_DIRS "${SOIL_INCLUDE_DIR}")
set(SOIL_LIBRARIES    "${SOIL_LIBRARY}")
mark_as_advanced(SOIL_INCLUDE_DIR SOIL_LIBRARY)

find_package_handle_standard_args(SOIL REQUIRED_VARS SOIL_LIBRARIES SOIL_INCLUDE_DIR ${CoreFoundation_DEPENDENCY_VAR}) #CoreFoundation is required only on macOS

if (NOT TARGET SOIL::SOIL)
	add_library(SOIL::SOIL UNKNOWN IMPORTED)
	set_target_properties(SOIL::SOIL PROPERTIES  INTERFACE_INCLUDE_DIRECTORIES "${SOIL_INCLUDE_DIRS}" IMPORTED_LOCATION "${SOIL_LIBRARIES}")
	set_property(TARGET SOIL::SOIL APPEND PROPERTY INTERFACE_LINK_LIBRARIES #[[OpenGL::GL]] "${OPENGL_LIBRARIES}") #the imported target is only available since CMake 3.8)
	set_property(TARGET SOIL::SOIL APPEND PROPERTY INTERFACE_LINK_LIBRARIES "${CoreFoundation_LIBRARY}")
endif()


