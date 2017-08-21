# FindOGG
# --------
#
# Find the Ogg bitstream and framing library
#
# IMPORTED Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the :prop_tgt:`IMPORTED` target ``OGG::OGG``,
# if OGG has been found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ::
#
#   OGG_INCLUDE_DIRS - include directories for OGG
#   OGG_LIBRARIES - libraries to link against OGG
#   OGG_DEFINITIONS - pre-processor definitions for OGG
#   OGG_FOUND - true if OGG has been found and can be used
#

if (OGG_FOUND)
	return()
endif()

include(SelectLibraryConfigurations)
include(FindPackageHandleStandardArgs)

if (WIN32)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(OGG_ARCH "x64")
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(OGG_ARCH "Win32")
	endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

	set(OGG_ROOT_DIR "${CMAKE_SOURCE_DIR}/ThirdParty/libogg" CACHE PATH "LibOGG root directory")
	set(INCLUDE_HINTS "${OGG_ROOT_DIR}/include")
	set(LIB_HINTS     "${OGG_ROOT_DIR}/lib/${OGG_ARCH}")
else ()
	find_package(PkgConfig)
	pkg_check_modules(PKG_OGG QUIET ogg>=1.3)

	set(OGG_DEFINITIONS ${PKG_OGG_CFLAGS})
	set(INCLUDE_HINTS   ${PKG_OGG_INCLUDEDIR} ${PKG_OGG_INCLUDE_DIRS})
	set(LIB_HINTS       ${PKG_OGG_LIBDIR}     ${PKG_OGG_LIBRARY_DIRS})
endif()

find_path(OGG_INCLUDE_DIR         NAMES ogg/ogg.h ogg/os_types.h    HINTS ${INCLUDE_HINTS})
find_library(OGG_LIBRARY_RELEASE  NAMES ogg libogg libogg_static    HINTS ${LIB_HINTS})
find_library(OGG_LIBRARY_DEBUG    NAMES ogg liboggd liboggd_static  HINTS ${LIB_HINTS})

set(OGG_INCLUDE_DIRS "${OGG_INCLUDE_DIR}")
mark_as_advanced(OGG_INCLUDE_DIR)
select_library_configurations(OGG)

find_package_handle_standard_args(OGG REQUIRED_VARS OGG_LIBRARY OGG_INCLUDE_DIRS)

if (NOT TARGET OGG::OGG)
	add_library(OGG::OGG UNKNOWN IMPORTED)
	set_target_properties(OGG::OGG PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${OGG_INCLUDE_DIRS}" )

	if(OGG_LIBRARY_RELEASE)
		set_property(TARGET OGG::OGG APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
		set_target_properties(OGG::OGG PROPERTIES IMPORTED_LOCATION_RELEASE "${OGG_LIBRARY_RELEASE}")
	endif()

	if(OGG_LIBRARY_DEBUG)
		set_property(TARGET SOIL::SOIL APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
		set_target_properties(OGG::OGG PROPERTIES IMPORTED_LOCATION_DEBUG "${OGG_LIBRARY_DEBUG}")
	endif()

	if(NOT OGG_LIBRARY_RELEASE AND NOT OGG_LIBRARY_DEBUG)
		set_property(TARGET OGG::OGG APPEND PROPERTY IMPORTED_LOCATION "${OGG_LIBRARY}")
	endif()
endif()
