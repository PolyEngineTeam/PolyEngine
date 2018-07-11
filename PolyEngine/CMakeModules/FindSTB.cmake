# FindSTB
# --------
#
# Find stb (single header library by Sean 'nothings' Bennets)
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
#   STB_INCLUDE_DIRS - include directories for stb
#   STB_FOUND - true if stb has been found and can be used

if (STB_FOUND)
	return()
endif()

include(FindPackageHandleStandardArgs)

if (WIN32)
	set(STB_ROOT_DIR "${ENGINE_ROOT_DIR}/ThirdParty/stb" CACHE PATH "stb root directory")
	set(INCLUDE_HINTS "${STB_ROOT_DIR}/include")
else ()
	find_package(PkgConfig)
	pkg_check_modules(PKG_STB QUIET STB>=1.0)

	set(STB_DEFINITIONS ${PKG_STB_CFLAGS})
	if (PKG_STB_FOUND)
		set(INCLUDE_HINTS ${PKG_STB_INCLUDEDIR} ${PKG_STB_INCLUDE_DIRS})
	else()
		set(INCLUDE_HINTS "${ENGINE_ROOT_DIR}/ThirdParty/stb/include")
	endif()
endif()

find_path(STB_INCLUDE_DIR  NAMES stb/stb_image.h  HINTS ${INCLUDE_HINTS})
set(STB_INCLUDE_DIRS "${STB_INCLUDE_DIR}")
mark_as_advanced(STB_INCLUDE_DIR)

find_package_handle_standard_args(STB  REQUIRED_VARS STB_INCLUDE_DIRS  VERSION_VAR PKG_STB_VERSION)

# if (NOT TARGET Rapid::JSON)
# 	add_library(Rapid::JSON INTERFACE IMPORTED)
# 	set_target_properties(Rapid::JSON PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${STB_INCLUDE_DIRS}")
# endif()
