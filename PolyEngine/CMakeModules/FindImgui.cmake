# FindImgui
# --------
#
# Find the Imgui
#
# IMPORTED Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the :prop_tgt:`IMPORTED` target ``Imgui::Imgui``,
# if Imgui has been found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ::
#
#   Imgui_INCLUDE_DIRS - include directories for Imgui
#   # Imgui_LIBRARIES - libraries to link against Imgui
#   Imgui_FOUND - true if Imgui has been found and can be used


if (Imgui_FOUND)
	return()
endif()

include(FindPackageHandleStandardArgs)

if (WIN32)
	# if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	# 	set(Imgui_ARCH "x64")
	# elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
	# 	set(Imgui_ARCH "Win32")
	# endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

	set(Imgui_ROOT_DIR "${ENGINE_ROOT_DIR}/ThirdParty/Imgui" CACHE PATH "Imgui root directory")

	find_path(Imgui_INCLUDE_DIRS
				NAMES "Imgui/Imgui.h" 
				PATHS "${Imgui_ROOT_DIR}" 
				PATH_SUFFIXES "Imgui" )
	# find_library(Imgui_LIBRARY  
	# 			Imgui
	# 			PATH  "${Imgui_ROOT_DIR}/lib/Release/${Imgui_ARCH}")
	# find_library(Imgui_LIBRARY_DEBUG
	# 			Imgui
	# 			PATH  "${Imgui_ROOT_DIR}/lib/Debug/${Imgui_ARCH}")
	
else()
	find_package(Imgui CONFIG QUIET) #note(vuko): frequently faulty, so try to use it just for `Imgui_ROOT_DIR` and `Imgui_VERSION_STRING`
	unset(Imgui_DIR CACHE)
	unset(Imgui_INCLUDE_DIRS)
	find_path(Imgui_INCLUDE_DIR  NAMES Imgui/Imgui.h  HINTS ${Imgui_ROOT_DIR})
	# find_library(Imgui_LIBRARY   NAMES Imgui          HINTS ${Imgui_ROOT_DIR}/lib/)
	# find_library(Imgui_LIBRARY_DEBUG   NAMES Imgui          HINTS ${Imgui_ROOT_DIR}/lib/)
endif()

set(Imgui_INCLUDE_DIRS "${Imgui_INCLUDE_DIR}")
# set(Imgui_LIBRARIES    "${Imgui_LIBRARY}")
# set(Imgui_LIBRARIES_DEBUG    "${Imgui_LIBRARY_DEBUG}")
# mark_as_advanced(Imgui_INCLUDE_DIR Imgui_LIBRARY Imgui_LIBRARY_DEBUG)
mark_as_advanced(Imgui_INCLUDE_DIR)

# if (DEFINED Imgui_VERSION_STRING AND (NOT Imgui_VERSION_STRING STREQUAL "") AND (Imgui_VERSION_STRING VERSION_LESS 2.3))
# 	message(FATAL_ERROR "Found Imgui ${Imgui_VERSION_STRING}, which is too old. Required version is 2.3+!")
# endif()
# find_package_handle_standard_args(Imgui  REQUIRED_VARS Imgui_LIBRARIES Imgui_INCLUDE_DIRS  VERSION_VAR Imgui_VERSION_STRING)
find_package_handle_standard_args(Imgui Imgui_INCLUDE_DIRS)

if (NOT TARGET Imgui::Imgui)
	add_library(Imgui::Imgui UNKNOWN IMPORTED)
	set_target_properties(Imgui::Imgui PROPERTIES  INTERFACE_INCLUDE_DIRECTORIES "${Imgui_INCLUDE_DIRS}"
	# IMPORTED_LOCATION "${Imgui_LIBRARIES}"
	# IMPORTED_LOCATION_DEBUG "${Imgui_LIBRARIES_DEBUG}"
	# IMPORTED_LOCATION_DEBUGFAST "${Imgui_LIBRARIES_DEBUG}"
	)	
endif()
