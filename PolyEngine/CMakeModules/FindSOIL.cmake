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
include(SelectLibraryConfigurations)
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

	set(SOIL_ROOT_DIR "${CMAKE_SOURCE_DIR}/ThirdParty/SOIL" CACHE PATH "SOIL root directory")
	set(INCLUDE_HINTS "${SOIL_ROOT_DIR}/include")
	set(LIB_HINTS     "${SOIL_ROOT_DIR}/lib/Release/${SOIL_ARCH}")
endif()

find_path(SOIL_INCLUDE_DIR         NAMES SOIL/SOIL.h  HINTS ${INCLUDE_HINTS})
find_library(SOIL_LIBRARY_RELEASE  NAMES SOIL soil    HINTS ${LIB_HINTS})
find_library(SOIL_LIBRARY_DEBUG    NAMES SOILd soild  HINTS ${LIB_HINTS})

set(SOIL_INCLUDE_DIRS "${SOIL_INCLUDE_DIR}")
mark_as_advanced(SOIL_INCLUDE_DIR)
select_library_configurations(SOIL)

find_package_handle_standard_args(SOIL REQUIRED_VARS SOIL_LIBRARY SOIL_INCLUDE_DIR)

if (NOT TARGET SOIL::SOIL)
	add_library(SOIL::SOIL UNKNOWN IMPORTED)
	set_target_properties(SOIL::SOIL PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${SOIL_INCLUDE_DIRS}")

	if(SOIL_LIBRARY_RELEASE)
		set_property(TARGET SOIL::SOIL APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
		set_target_properties(SOIL::SOIL PROPERTIES IMPORTED_LOCATION_RELEASE "${SOIL_LIBRARY_RELEASE}")
	endif()

	if(SOIL_LIBRARY_DEBUG)
		set_property(TARGET SOIL::SOIL APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
		set_target_properties(SOIL::SOIL PROPERTIES IMPORTED_LOCATION_DEBUG "${SOIL_LIBRARY_DEBUG}")
	endif()

	if(NOT SOIL_LIBRARY_RELEASE AND NOT SOIL_LIBRARY_DEBUG)
		set_property(TARGET SOIL::SOIL APPEND PROPERTY IMPORTED_LOCATION "${SOIL_LIBRARY}")
	endif()

	set_property(TARGET SOIL::SOIL APPEND PROPERTY INTERFACE_LINK_LIBRARIES "${OPENGL_LIBRARIES}")
endif()


