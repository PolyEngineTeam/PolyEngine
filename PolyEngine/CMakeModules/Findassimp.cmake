# Findassimp
# --------
#
# Find the Open Asset Import Library (assimp)
#
# Inspired by the official version in the assimp repository (rev 270355f).
# License file can be found in ThirdParty/assimp.
#
# IMPORTED Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the :prop_tgt:`IMPORTED` target ``ass::imp``,
# if assimp has been found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ::
#
#   assimp_INCLUDE_DIRS - include directories for assimp
#   assimp_LIBRARIES - libraries to link against assimp
#   assimp_DEFINITIONS - pre-processor definitions for assimp
#   assimp_FOUND - true if assimp has been found and can be used
#

#NOTE(vuko): older versions of assimp unfortunately do not provide an assimp-config.cmake file
#            which is why we need this

if (assimp_FOUND)
	return()
endif()

include(FindPackageHandleStandardArgs)

if (WIN32)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(assimp_ARCH "x64")
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(assimp_ARCH "Win32")
	endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

	if(MSVC12)
		set(assimp_MSVC_VERSION "vc120")
	elseif(MSVC14)
		set(assimp_MSVC_VERSION "vc140")
	endif(MSVC12)

	set(assimp_ROOT_DIR "${ENGINE_ROOT_DIR}/ThirdParty/assimp" CACHE PATH "Assimp root directory")
	set(INCLUDE_HINTS "${assimp_ROOT_DIR}/include")
	set(LIB_HINTS     "${assimp_ROOT_DIR}/lib/Release/${assimp_ARCH}")
	set(LIB_NAMES     "assimp-${assimp_MSVC_VERSION}-mt")
else()
	find_package(PkgConfig)
	pkg_search_module(PKG_assimp QUIET assimp>=3.0 assimp3.0)

	set(assimp_DEFINITIONS ${PKG_assimp_CFLAGS})
	set(INCLUDE_HINTS ${PKG_assimp_INCLUDEDIR} ${PKG_assimp_INCLUDE_DIRS})
	set(LIB_HINTS     ${PKG_assimp_LIBDIR}     ${PKG_assimp_LIBRARY_DIRS})
	set(LIB_NAMES     assimp assimp3.0)
endif()

find_path(assimp_INCLUDE_DIR  NAMES assimp/postprocess.h assimp/scene.h assimp/version.h assimp/config.h assimp/cimport.h  HINTS ${INCLUDE_HINTS})
find_library(assimp_LIBRARY   NAMES ${LIB_NAMES}                                                                           HINTS ${LIB_HINTS})

set(assimp_INCLUDE_DIRS "${assimp_INCLUDE_DIR}")
set(assimp_LIBRARIES    "${assimp_LIBRARY}")
mark_as_advanced(assimp_INCLUDE_DIR assimp_LIBRARY)

if (DEFINED PKG_assimp_VERSION AND (PKG_assimp_VERSION VERSION_LESS 3.0))
	message(FATAL_ERROR "Found Assimp ${PKG_assimp_VERSION}, which is too old. Required version is 3+!")
endif()
find_package_handle_standard_args(assimp  REQUIRED_VARS assimp_LIBRARIES assimp_INCLUDE_DIRS  VERSION_VAR PKG_assimp_VERSION)

if (NOT TARGET ass::imp)
	add_library(ass::imp UNKNOWN IMPORTED)
	set_target_properties(ass::imp PROPERTIES  INTERFACE_INCLUDE_DIRECTORIES "${assimp_INCLUDE_DIRS}"  IMPORTED_LOCATION "${assimp_LIBRARIES}")
endif()
