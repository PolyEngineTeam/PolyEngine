# FindSOIL
# --------
#
# Find the Simple OpenGL Image Library (SOIL)
#
# IMPORTED Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the :prop_tgt:`IMPORTED` target ``SOIL::SOIL``,
# if SOIL has been found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ::
#
#   SOIL_INCLUDE_DIRS - include directories for SOIL
#   SOIL_LIBRARIES - libraries to link against SOIL
#   SOIL_FOUND - true if SOIL has been found and can be used

find_path(SOIL_INCLUDE_DIR SOIL/SOIL.h)

if(NOT SOIL_LIBRARY)
	find_library(SOIL_LIBRARY_RELEASE NAMES SOIL soil PATH_SUFFIXES lib64)
	find_library(SOIL_LIBRARY_DEBUG NAMES SOILd soild PATH_SUFFIXES lib64)

	include(SelectLibraryConfigurations)
	select_library_configurations(SOIL)
endif ()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SOIL REQUIRED_VARS SOIL_INCLUDE_DIR SOIL_LIBRARY)

if(SOIL_FOUND)
	set(SOIL_INCLUDE_DIRS ${SOIL_INCLUDE_DIR})

	if(NOT SOIL_LIBRARIES)
		set(SOIL_LIBRARIES ${SOIL_LIBRARY})
	endif()

	if (NOT TARGET SOIL::SOIL)
		add_library(SOIL::SOIL UNKNOWN IMPORTED)
		set_target_properties(SOIL::SOIL PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${SOIL_INCLUDE_DIRS}")

		if(SOIL_LIBRARY_RELEASE)
			set_property(TARGET SOIL::SOIL APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
			set_target_properties(SOIL::SOIL PROPERTIES IMPORTED_LOCATION_RELEASE "${SOIL_LIBRARY_RELEASE}")
		endif()

		if(SOIL_LIBRARY_DEBUG)
			set_property(TARGET SOIL::SOIL APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
			set_target_properties(SOIL::SOIL PROPERTIES IMPORTED_LOCATION_DEBUG "${SOIL_LIBRARY_DEBUG}")
		endif()

		if(NOT SOIL_LIBRARY_RELEASE AND NOT SOIL_LIBRARY_DEBUG)
			set_property(TARGET SOIL::SOIL APPEND PROPERTY IMPORTED_LOCATION "${SOIL_LIBRARY}")
		endif()
	endif()
endif()

mark_as_advanced(SOIL_INCLUDE_DIR)

