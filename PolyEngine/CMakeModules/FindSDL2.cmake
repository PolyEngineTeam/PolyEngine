# FindSDL2
# --------
#
# Find the SDL2 physics engine for games
#
# IMPORTED Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the :prop_tgt:`IMPORTED` target ``SDL2::SDL2``,
# if SDL2 has been found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ::
#
#   SDL2_INCLUDE_DIRS - include directories for SDL2
#   SDL2_LIBRARIES - libraries to link against SDL2
#   SDL2_FOUND - true if SDL2 has been found and can be used


if (SDL2_FOUND)
	return()
endif()

include(FindPackageHandleStandardArgs)

if (WIN32)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(SDL2_ARCH "x64")
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(SDL2_ARCH "Win32")
	endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

	set(SDL2_ROOT_DIR "${CMAKE_SOURCE_DIR}/ThirdParty/SDL2" CACHE PATH "SDL2 root directory")

	find_path(SDL2_INCLUDE_DIR "SDL.h" PATHS "${SDL2_ROOT_DIR}/include" )
	find_library(SDL2_LIBRARY SDL2 PATH "${SDL2_ROOT_DIR}/lib/${SDL2_ARCH}" NO_DEFAULT_PATH)
	find_library(SDL2main_LIBRARY SDL2main PATH "${SDL2_ROOT_DIR}/lib/${SDL2_ARCH}" NO_DEFAULT_PATH)
	
else()
	find_package(SDL2 CONFIG QUIET) #note(vuko): frequently faulty, so try to use it just for `SDL2_ROOT_DIR` and `SDL2_VERSION_STRING`
	unset(SDL2_DIR CACHE)
	unset(SDL2_INCLUDE_DIRS)
	find_path(SDL2_INCLUDE_DIR  NAMES SDL2/SDL2.h  HINTS ${SDL2_ROOT_DIR}/include)
	find_library(SDL2_LIBRARY   NAMES SDL2          HINTS ${SDL2_ROOT_DIR}/lib/)
endif()

set(SDL2_INCLUDE_DIRS "${SDL2_INCLUDE_DIR}")
set(SDL2_LIBRARIES    "${SDL2_LIBRARY}" "${SDL2main_LIBRARY}")
mark_as_advanced(SDL2_INCLUDE_DIR SDL2_LIBRARY SDL2main_LIBRARY)

if (DEFINED SDL2_VERSION_STRING AND (NOT SDL2_VERSION_STRING STREQUAL "") AND (SDL2_VERSION_STRING VERSION_LESS 2.3))
	message(FATAL_ERROR "Found SDL2 ${SDL2_VERSION_STRING}, which is too old. Required version is 2.3+!")
endif()
find_package_handle_standard_args(SDL2  REQUIRED_VARS SDL2_LIBRARIES SDL2_INCLUDE_DIRS  VERSION_VAR SDL2_VERSION_STRING)

if (NOT TARGET SDL2::SDL2)
	add_library(SDL2::SDL2 UNKNOWN IMPORTED)
	set_target_properties(SDL2::SDL2 PROPERTIES  INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIRS}"  IMPORTED_LOCATION "${SDL2_LIBRARIES}")
endif()
