# FindFreetype
# --------
#
# Find the Freetype software font rendering library
#
# IMPORTED Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the :prop_tgt:`IMPORTED` target ``Freetype::FT2``,
# if Freetype has been found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ::
#
#   Freetype_INCLUDE_DIRS - include directories for Freetype
#   Freetype_LIBRARIES - libraries to link against Freetype
#   Freetype_FOUND - true if Freetype has been found and can be used

if (Freetype_FOUND)
	return()
endif()

if (WIN32)
	include(FindPackageHandleStandardArgs)

	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(Freetype_ARCH "x64")
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(Freetype_ARCH "Win32")
	endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

	set(Freetype_ROOT_DIR "${ENGINE_ROOT_DIR}/ThirdParty/freetype" CACHE PATH "Freetype root directory")

	find_path(Freetype_INCLUDE_DIR  NAMES freetype/freetype.h  HINTS "${Freetype_ROOT_DIR}/include")
	find_library(Freetype_LIBRARY   NAMES freetype freetype28  HINTS "${Freetype_ROOT_DIR}/lib/Release/${Freetype_ARCH}")

	set(Freetype_INCLUDE_DIRS "${Freetype_INCLUDE_DIR}")
	set(Freetype_LIBRARIES    "${Freetype_LIBRARY}")
	mark_as_advanced(Freetype_INCLUDE_DIR Freetype_LIBRARY)

	find_package_handle_standard_args(Freetype REQUIRED_VARS Freetype_LIBRARIES Freetype_INCLUDE_DIRS)
else()
	set(CMAKE_MODULE_PATH_BACKUP "${CMAKE_MODULE_PATH}")
	set(CMAKE_MODULE_PATH)
	find_package(Freetype MODULE REQUIRED)
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH_BACKUP})
	set(Freetype_INCLUDE_DIRS "${FREETYPE_INCLUDE_DIRS}")
	set(Freetype_LIBRARY   "${FREETYPE_LIBRARIES}")
	set(Freetype_LIBRARIES "${FREETYPE_LIBRARIES}")
	set(Freetype_FOUND "${FREETYPE_FOUND}")
	mark_as_advanced(FREETYPE_LIBRARIES Freetype_LIBRARY FREETYPE_INCLUDE_DIRS)
endif()

if (NOT TARGET Freetype::FT2)
	add_library(Freetype::FT2 UNKNOWN IMPORTED)
	set_target_properties(Freetype::FT2 PROPERTIES  INTERFACE_INCLUDE_DIRECTORIES "${Freetype_INCLUDE_DIRS}"  IMPORTED_LOCATION "${Freetype_LIBRARIES}")
endif()
