# FindOpenAL
# --------
#
# Find the Open Audio Library (OpenAL)
#
# IMPORTED Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the :prop_tgt:`IMPORTED` target ``OpenAL::AL``,
# if OpenAL has been found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ::
#
#   OpenAL_INCLUDE_DIRS - include directories for OpenAL
#   OpenAL_LIBRARIES - libraries to link against OpenAL
#   OpenAL_FOUND - true if OpenAL has been found and can be used

if (OpenAL_FOUND)
	return()
endif()

if (WIN32)
	include(SelectLibraryConfigurations)
	include(FindPackageHandleStandardArgs)

	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(OpenAL_ARCH "Win64")
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(OpenAL_ARCH "Win32")
	endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

	set(OpenAL_ROOT_DIR "${CMAKE_SOURCE_DIR}/ThirdParty/OpenAL" CACHE PATH "OpenAL root directory")

	find_path(OpenAL_INCLUDE_DIR NAMES   NAMES al.h alc.h  HINTS "${OpenAL_ROOT_DIR}/include")
	find_library(OpenAL_LIBRARY_RELEASE  NAMES OpenAL32    HINTS "${OpenAL_ROOT_DIR}/lib/${OpenAL_ARCH}")
	find_library(OpenAL_LIBRARY_DEBUG    NAMES OpenAL32d   HINTS "${OpenAL_ROOT_DIR}/lib/${OpenAL_ARCH}")

	set(OpenAL_INCLUDE_DIRS "${OpenAL_INCLUDE_DIR}")
	mark_as_advanced(OpenAL_INCLUDE_DIR)
	select_library_configurations(OpenAL)

	find_package_handle_standard_args(OpenAL REQUIRED_VARS OpenAL_LIBRARY OpenAL_INCLUDE_DIRS)
else()
	set(CMAKE_MODULE_PATH_BACKUP "${CMAKE_MODULE_PATH}")
	set(CMAKE_MODULE_PATH)
	find_package(OpenAL MODULE REQUIRED)
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH_BACKUP})
	set(OpenAL_INCLUDE_DIRS "${OPENAL_INCLUDE_DIR}")
	set(OpenAL_LIBRARY   "${OPENAL_LIBRARY}")
	set(OpenAL_LIBRARIES "${OPENAL_LIBRARY}")
	set(OpenAL_FOUND "${OPENAL_FOUND}")
	mark_as_advanced(OPENAL_LIBRARY OpenAL_LIBRARY OPENAL_INCLUDE_DIR)
endif()

if (NOT TARGET OpenAL::AL)
	add_library(OpenAL::AL UNKNOWN IMPORTED)
	set_target_properties(OpenAL::AL PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${OpenAL_INCLUDE_DIRS}")

	if(OpenAL_LIBRARY_RELEASE)
		set_property(TARGET OpenAL::AL APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
		set_target_properties(OpenAL::AL PROPERTIES IMPORTED_LOCATION_RELEASE "${OpenAL_LIBRARY_RELEASE}")
	endif()

	if(OpenAL_LIBRARY_DEBUG)
		set_property(TARGET OpenAL::AL APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
		set_target_properties(OpenAL::AL PROPERTIES IMPORTED_LOCATION_DEBUG "${OpenAL_LIBRARY_DEBUG}")
	endif()

	if(NOT OpenAL_LIBRARY_RELEASE AND NOT OpenAL_LIBRARY_DEBUG)
		set_property(TARGET OpenAL::AL APPEND PROPERTY IMPORTED_LOCATION "${OpenAL_LIBRARY}")
	endif()
endif()
