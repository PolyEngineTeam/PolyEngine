# Findepoxy
# --------
#
# Find epoxy (library for handling OpenGL function pointer management)
#
# IMPORTED Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the :prop_tgt:`IMPORTED` target ``epoxy::gl``,
# if epoxy has been found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ::
#
#   epoxy_INCLUDE_DIRS - include directories for epoxy
#   epoxy_LIBRARIES - libraries to link against epoxy
#   epoxy_DEFINITIONS - pre-processor definitions for epoxy
#   epoxy_FOUND - true if epoxy has been found and can be used

if (epoxy_FOUND)
	return()
endif()

include(FindPackageHandleStandardArgs)

if (WIN32)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(epoxy_ARCH "x64")
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(epoxy_ARCH "Win32")
	endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

	set(epoxy_ROOT_DIR "${CMAKE_SOURCE_DIR}/ThirdParty/libepoxy" CACHE PATH "Epoxy root directory")
	set(INCLUDE_HINTS "${epoxy_ROOT_DIR}/include")
	set(LIB_HINTS     "${epoxy_ROOT_DIR}/lib/${epoxy_ARCH}")
else ()
	find_package(PkgConfig)
	pkg_check_modules(PKG_epoxy QUIET epoxy>=1.1)

	set(epoxy_DEFINITIONS ${PKG_epoxy_CFLAGS})
	set(INCLUDE_HINTS ${PKG_epoxy_INCLUDEDIR} ${PKG_epoxy_INCLUDE_DIRS})
	set(LIB_HINTS     ${PKG_epoxy_LIBDIR}     ${PKG_epoxy_LIBRARY_DIRS})
endif()

find_path(epoxy_INCLUDE_DIR  NAMES epoxy/gl.h   HINTS ${INCLUDE_HINTS})
find_library(epoxy_LIBRARY   NAMES epoxy        HINTS ${LIB_HINTS})
find_file(epoxy_GLX_HEADER   NAMES epoxy/glx.h  HINTS ${epoxy_INCLUDE_DIR})
find_file(epoxy_WGL_HEADER   NAMES epoxy/wgl.h  HINTS ${epoxy_INCLUDE_DIR})

set(epoxy_INCLUDE_DIRS "${epoxy_INCLUDE_DIR}")
set(epoxy_LIBRARIES    "${epoxy_LIBRARY}")
mark_as_advanced(epoxy_INCLUDE_DIR epoxy_LIBRARY epoxy_GLX_HEADER epoxy_WGL_HEADER)

find_package_handle_standard_args(epoxy REQUIRED_VARS epoxy_LIBRARY epoxy_INCLUDE_DIRS)

if (NOT TARGET epoxy::gl)
	add_library(epoxy::gl UNKNOWN IMPORTED)
	set_target_properties(epoxy::gl PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${epoxy_INCLUDE_DIRS}" IMPORTED_LOCATION "${epoxy_LIBRARY}")
endif()

