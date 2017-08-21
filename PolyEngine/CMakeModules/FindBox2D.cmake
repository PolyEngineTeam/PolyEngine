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
#   BOX2D_INCLUDE_DIRS - include directories for Box2D
#   BOX2D_LIBRARIES - libraries to link against Box2D
#   BOX2D_FOUND - true if Box2D has been found and can be used


if (BOX2D_FOUND)
	return()
endif()


if (WIN32)
	include(SelectLibraryConfigurations)
	include(FindPackageHandleStandardArgs)

	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(BOX2D_ARCH "x64")
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(BOX2D_ARCH "Win32")
	endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

	set(BOX2D_ROOT_DIR "${CMAKE_SOURCE_DIR}/ThirdParty/Box2D" CACHE PATH "Box2D root directory")

	find_path(BOX2D_INCLUDE_DIR NAMES   NAMES Box2D/Box2D.h  HINTS "${BOX2D_ROOT_DIR}/Src")
	find_library(BOX2D_LIBRARY_RELEASE  NAMES Box2D          HINTS "${BOX2D_ROOT_DIR}/lib/Release/${BOX2D_ARCH}")
#	find_library(BOX2D_LIBRARY_DEBUG    NAMES Box2D          HINTS "${BOX2D_ROOT_DIR}/lib/Debug/${BOX2D_ARCH}")    #todo(vuko/muniu): breaks the build; why?

	set(BOX2D_INCLUDE_DIRS "${BOX2D_INCLUDE_DIR}")
	mark_as_advanced(BOX2D_INCLUDE_DIR)
	select_library_configurations(BOX2D)

	find_package_handle_standard_args(Box2D REQUIRED_VARS BOX2D_LIBRARY BOX2D_INCLUDE_DIRS)
else()
	find_package(Box2D CONFIG REQUIRED)
	unset(Box2D_DIR CACHE)
	unset(BOX2D_LIBRARY)
	find_library(BOX2D_LIBRARY  NAMES Box2D  HINTS ${BOX2D_ROOT_DIR}/lib/)
	set(BOX2D_LIBRARIES "${BOX2D_LIBRARY}")
	mark_as_advanced(BOX2D_LIBRARY)
endif()

if (NOT TARGET Box2D::Box2D)
	add_library(Box2D::Box2D UNKNOWN IMPORTED)
	set_target_properties(Box2D::Box2D PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${BOX2D_INCLUDE_DIRS}")

	if(BOX2D_LIBRARY_RELEASE)
		set_property(TARGET Box2D::Box2D APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
		set_target_properties(Box2D::Box2D PROPERTIES IMPORTED_LOCATION_RELEASE "${BOX2D_LIBRARY_RELEASE}")
	endif()

	if(BOX2D_LIBRARY_DEBUG)
		set_property(TARGET Box2D::Box2D APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
		set_target_properties(Box2D::Box2D PROPERTIES IMPORTED_LOCATION_DEBUG "${BOX2D_LIBRARY_DEBUG}")
	endif()

	if(NOT BOX2D_LIBRARY_RELEASE AND NOT BOX2D_LIBRARY_DEBUG)
		set_property(TARGET Box2D::Box2D APPEND PROPERTY IMPORTED_LOCATION "${BOX2D_LIBRARY}")
	endif()
endif()
