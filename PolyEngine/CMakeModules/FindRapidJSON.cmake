# FindRapidJSON
# --------
#
# Find RapidJSON (a fast JSON parser/generator for C++ with both SAX/DOM style API)
#
# IMPORTED Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the :prop_tgt:`IMPORTED` target ``Rapid::JSON``,
# if RapidJSON has been found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ::
#
#   RapidJSON_INCLUDE_DIRS - include directories for RapidJSON
#   RapidJSON_DEFINITIONS - pre-processor definitions for RapidJSON
#   RapidJSON_FOUND - true if RapidJSON has been found and can be used

if (RapidJSON_FOUND)
	return()
endif()

include(FindPackageHandleStandardArgs)

if (WIN32)
	set(RapidJSON_ROOT_DIR "${CMAKE_SOURCE_DIR}/ThirdParty/rapidjson" CACHE PATH "RapidJSON root directory")
	set(INCLUDE_HINTS "${RapidJSON_ROOT_DIR}/include")
else ()
	find_package(PkgConfig)
	pkg_check_modules(PKG_RapidJSON QUIET RapidJSON>=1.0)

	set(RapidJSON_DEFINITIONS ${PKG_RapidJSON_CFLAGS})
	if (PKG_RapidJSON_FOUND)
		set(INCLUDE_HINTS ${PKG_RapidJSON_INCLUDEDIR} ${PKG_RapidJSON_INCLUDE_DIRS})
	else()
		set(INCLUDE_HINTS "${CMAKE_SOURCE_DIR}/ThirdParty/rapidjson/include")
	endif()
endif()

find_path(RapidJSON_INCLUDE_DIR  NAMES rapidjson/rapidjson.h  HINTS ${INCLUDE_HINTS})
set(RapidJSON_INCLUDE_DIRS "${RapidJSON_INCLUDE_DIR}")
mark_as_advanced(RapidJSON_INCLUDE_DIR)

find_package_handle_standard_args(RapidJSON REQUIRED_VARS RapidJSON_INCLUDE_DIRS)

if (NOT TARGET Rapid::JSON)
	add_library(Rapid::JSON INTERFACE IMPORTED)
	set_target_properties(Rapid::JSON PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${RapidJSON_INCLUDE_DIRS}")
endif()
