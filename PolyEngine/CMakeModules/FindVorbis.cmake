# FindVorbis
# --------
#
# Find the Vorbis codec library
#
# IMPORTED Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the :prop_tgt:`IMPORTED` targets
# ``Vorbis::Vorbis``, if Vorbis has been found.
# ``Vorbis::File``, if VorbisFile has been found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ::
#
#   Vorbis_INCLUDE_DIRS - include directories for Vorbis
#   Vorbis_LIBRARIES - libraries to link against Vorbis
#   Vorbis_DEFINITIONS - pre-processor definitions for Vorbis
#   Vorbis_FOUND - true if Vorbis has been found and can be used
#
#   VorbisFile_INCLUDE_DIRS - include directories for VorbisFile
#   VorbisFile_LIBRARIES - libraries to link against VorbisFile
#   VorbisFile_DEFINITIONS - pre-processor definitions for VorbisFile
#   VorbisFile_FOUND - true if VorbisFile has been found and can be used
#

if (Vorbis_FOUND)
	return()
endif()

include(SelectLibraryConfigurations)
include(FindPackageHandleStandardArgs)

if (WIN32)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(Vorbis_ARCH "x64")
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(Vorbis_ARCH "Win32")
	endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

	set(Vorbis_ROOT_DIR "${CMAKE_SOURCE_DIR}/ThirdParty/libvorbis" CACHE PATH "LibVorbis root directory")
	set(INCLUDE_HINTS   "${Vorbis_ROOT_DIR}/include")
	set(LIB_HINTS       "${Vorbis_ROOT_DIR}/lib/${Vorbis_ARCH}")
else ()
	find_package(PkgConfig)
	pkg_check_modules(PKG_Vorbis QUIET vorbis>=1.3)

	set(Vorbis_DEFINITIONS ${PKG_Vorbis_CFLAGS})
	set(INCLUDE_HINTS   ${PKG_Vorbis_INCLUDEDIR} ${PKG_Vorbis_INCLUDE_DIRS})
	set(LIB_HINTS       ${PKG_Vorbis_LIBDIR}     ${PKG_Vorbis_LIBRARY_DIRS})
endif()

find_path(Vorbis_INCLUDE_DIR         NAMES vorbis/codec.h                        HINTS ${INCLUDE_HINTS})
find_library(Vorbis_LIBRARY_RELEASE  NAMES vorbis libvorbis libvorbis_static     HINTS ${LIB_HINTS})
find_library(Vorbis_LIBRARY_DEBUG    NAMES vorbisd libvorbisd libvorbisd_static  HINTS ${LIB_HINTS})

set(Vorbis_INCLUDE_DIRS "${Vorbis_INCLUDE_DIR}")
mark_as_advanced(Vorbis_INCLUDE_DIR)
select_library_configurations(Vorbis)

find_package_handle_standard_args(Vorbis REQUIRED_VARS Vorbis_LIBRARY Vorbis_INCLUDE_DIRS)

if (NOT TARGET Vorbis::Vorbis)
	add_library(Vorbis::Vorbis UNKNOWN IMPORTED)
	set_target_properties(Vorbis::Vorbis PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${Vorbis_INCLUDE_DIRS}" )

	if(Vorbis_LIBRARY_RELEASE)
		set_property(TARGET Vorbis::Vorbis APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
		set_target_properties(Vorbis::Vorbis PROPERTIES IMPORTED_LOCATION_RELEASE "${Vorbis_LIBRARY_RELEASE}")
	endif()

	if(Vorbis_LIBRARY_DEBUG)
		set_property(TARGET Vorbis::Vorbis APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
		set_target_properties(Vorbis::Vorbis PROPERTIES IMPORTED_LOCATION_DEBUG "${Vorbis_LIBRARY_DEBUG}")
	endif()

	if(NOT Vorbis_LIBRARY_RELEASE AND NOT Vorbis_LIBRARY_DEBUG)
		set_property(TARGET Vorbis::Vorbis APPEND PROPERTY IMPORTED_LOCATION "${Vorbis_LIBRARY}")
	endif()
endif()

find_path(VorbisFile_INCLUDE_DIR         NAMES vorbis/vorbisfile.h                               HINTS ${INCLUDE_HINTS})
find_library(VorbisFile_LIBRARY_RELEASE  NAMES vorbisfile libvorbisfile libvorbisfile_static     HINTS ${LIB_HINTS})
find_library(VorbisFile_LIBRARY_DEBUG    NAMES vorbisfiled libvorbisfiled libvorbisfiled_static  HINTS ${LIB_HINTS})

set(VorbisFile_INCLUDE_DIRS "${VorbisFile_INCLUDE_DIR}")
mark_as_advanced(VorbisFile_INCLUDE_DIR)
select_library_configurations(VorbisFile)

find_package_handle_standard_args(VorbisFile REQUIRED_VARS VorbisFile_LIBRARY VorbisFile_INCLUDE_DIRS)

if (NOT TARGET Vorbis::File)
	add_library(Vorbis::File UNKNOWN IMPORTED)
	set_target_properties(Vorbis::File PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${VorbisFile_INCLUDE_DIRS}" )

	if(Vorbis_LIBRARY_RELEASE)
		set_property(TARGET Vorbis::File APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
		set_target_properties(Vorbis::File PROPERTIES IMPORTED_LOCATION_RELEASE "${VorbisFile_LIBRARY_RELEASE}")
	endif()

	if(Vorbis_LIBRARY_DEBUG)
		set_property(TARGET Vorbis::File APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
		set_target_properties(Vorbis::File PROPERTIES IMPORTED_LOCATION_DEBUG "${VorbisFile_LIBRARY_DEBUG}")
	endif()

	if(NOT Vorbis_LIBRARY_RELEASE AND NOT Vorbis_LIBRARY_DEBUG)
		set_property(TARGET Vorbis::File APPEND PROPERTY IMPORTED_LOCATION "${VorbisFile_LIBRARY}")
	endif()
endif()
