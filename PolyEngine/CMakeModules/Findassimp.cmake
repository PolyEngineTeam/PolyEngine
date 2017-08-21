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

include(SelectLibraryConfigurations)
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

	set(assimp_ROOT_DIR "${CMAKE_SOURCE_DIR}/ThirdParty/assimp" CACHE PATH "Assimp root directory")
	set(INCLUDE_HINTS "${assimp_ROOT_DIR}/include")
	set(LIB_HINTS     "${assimp_ROOT_DIR}/lib/Release/${assimp_ARCH}" "${assimp_ROOT_DIR}/lib/Debug/${assimp_ARCH}")
	set(LIB_NAMES     "assimp-${assimp_MSVC_VERSION}-mt")
	set(LIB_NAMES_DBG "assimp-${assimp_MSVC_VERSION}-mtd")
else()
	find_package(PkgConfig)
	pkg_search_module(PKG_assimp QUIET assimp>=3.0 assimp3.0)

	set(INCLUDE_HINTS ${PKG_assimp_INCLUDEDIR} ${PKG_assimp_INCLUDE_DIRS})
	set(LIB_HINTS     ${PKG_assimp_LIBDIR}     ${PKG_assimp_LIBRARY_DIRS})
	set(assimp_DEFINITIONS ${PKG_assimp_CFLAGS})
	set(LIB_NAMES     assimp assimp3.0)
	set(LIB_NAMES_DBG assimp-dbg)
endif()

find_path(assimp_INCLUDE_DIR        NAMES assimp/postprocess.h assimp/scene.h assimp/version.h assimp/config.h assimp/cimport.h  HINTS ${INCLUDE_HINTS})
find_library(assimp_LIBRARY_RELEASE NAMES ${LIB_NAMES}                                                                           HINTS ${LIB_HINTS})
find_library(assimp_LIBRARY_DEBUG   NAMES ${LIB_NAMES_DBG}                                                                       HINTS ${LIB_HINTS})

set(assimp_INCLUDE_DIRS "${assimp_INCLUDE_DIR}")
mark_as_advanced(assimp_INCLUDE_DIR)
select_library_configurations(assimp)

if (NOT assimp_INCLUDE_DIRS AND assimp_LIBRARIES)
	message(FATAL_ERROR "Found Assimp 2? Required version is 3! (or we just couldn't find the headers)")
endif()
find_package_handle_standard_args(assimp REQUIRED_VARS assimp_LIBRARY assimp_INCLUDE_DIRS)

if (NOT TARGET ass::imp)
	add_library(ass::imp UNKNOWN IMPORTED)
	set_target_properties(ass::imp PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${assimp_INCLUDE_DIRS}")

	if(assimp_LIBRARY_RELEASE)
		set_property(TARGET ass::imp APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
		set_target_properties(ass::imp PROPERTIES IMPORTED_LOCATION_RELEASE "${assimp_LIBRARY_RELEASE}")
	endif()

	if(assimp_LIBRARY_DEBUG)
		set_property(TARGET ass::imp APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
		set_target_properties(ass::imp PROPERTIES IMPORTED_LOCATION_DEBUG "${assimp_LIBRARY_DEBUG}")
	endif()

	if(NOT assimp_LIBRARY_RELEASE AND NOT assimp_LIBRARY_DEBUG)
		set_property(TARGET ass::imp APPEND PROPERTY IMPORTED_LOCATION "${assimp_LIBRARY}")
	endif()
endif()
