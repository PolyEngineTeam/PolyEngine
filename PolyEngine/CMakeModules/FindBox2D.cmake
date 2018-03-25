# FindBox2D
# --------
#
# Find the Box2D physics engine for games
#
# IMPORTED Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the :prop_tgt:`IMPORTED` target ``Box2D::Box2D``,
# if Box2D has been found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ::
#
#   Box2D_INCLUDE_DIRS - include directories for Box2D
#   Box2D_LIBRARIES - libraries to link against Box2D
#   Box2D_FOUND - true if Box2D has been found and can be used


if (BOX2D_FOUND)
	return()
endif()

include(FindPackageHandleStandardArgs)

if (WIN32)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(Box2D_ARCH "x64")
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(Box2D_ARCH "Win32")
	endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

	set(Box2D_ROOT_DIR "${ENGINE_ROOT_DIR}/ThirdParty/Box2D" CACHE PATH "Box2D root directory")

	find_path(Box2D_INCLUDE_DIR
				NAMES "Box2D/Box2D.h" 
				PATHS "${Box2D_ROOT_DIR}/Src" 
				PATH_SUFFIXES "Box2D" )
	find_library(Box2D_LIBRARY  
				Box2D
				PATH  "${Box2D_ROOT_DIR}/lib/Release/${Box2D_ARCH}")
	find_library(Box2D_LIBRARY_DEBUG
				Box2D
				PATH  "${Box2D_ROOT_DIR}/lib/Debug/${Box2D_ARCH}")
	
else()
	find_package(Box2D CONFIG QUIET) #note(vuko): frequently faulty, so try to use it just for `BOX2D_ROOT_DIR` and `BOX2D_VERSION_STRING`
	unset(Box2D_DIR CACHE)
	unset(Box2D_INCLUDE_DIRS)
	find_path(Box2D_INCLUDE_DIR  NAMES Box2D/Box2D.h  HINTS ${BOX2D_ROOT_DIR}/include)
	find_library(Box2D_LIBRARY   NAMES Box2D          HINTS ${BOX2D_ROOT_DIR}/lib/)
	find_library(Box2D_LIBRARY_DEBUG   NAMES Box2D          HINTS ${BOX2D_ROOT_DIR}/lib/)
endif()

set(Box2D_INCLUDE_DIRS "${Box2D_INCLUDE_DIR}")
set(Box2D_LIBRARIES    "${Box2D_LIBRARY}")
set(Box2D_LIBRARIES_DEBUG    "${Box2D_LIBRARY_DEBUG}")
mark_as_advanced(Box2D_INCLUDE_DIR Box2D_LIBRARY Box2D_LIBRARY_DEBUG)

if (DEFINED BOX2D_VERSION_STRING AND (NOT BOX2D_VERSION_STRING STREQUAL "") AND (BOX2D_VERSION_STRING VERSION_LESS 2.3))
	message(FATAL_ERROR "Found Box2D ${BOX2D_VERSION_STRING}, which is too old. Required version is 2.3+!")
endif()
find_package_handle_standard_args(Box2D  REQUIRED_VARS Box2D_LIBRARIES Box2D_INCLUDE_DIRS  VERSION_VAR BOX2D_VERSION_STRING)

if (NOT TARGET Box2D::Box2D)
	add_library(Box2D::Box2D UNKNOWN IMPORTED)
	set_target_properties(Box2D::Box2D PROPERTIES  INTERFACE_INCLUDE_DIRECTORIES "${Box2D_INCLUDE_DIRS}"  
	IMPORTED_LOCATION "${Box2D_LIBRARIES}"
	IMPORTED_LOCATION_DEBUG "${Box2D_LIBRARIES_DEBUG}"
	IMPORTED_LOCATION_DEBUGFAST "${Box2D_LIBRARIES_DEBUG}")
endif()
