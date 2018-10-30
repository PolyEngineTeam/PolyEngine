# FindImgui
# --------
#
# Find Imgui (Dear ImGui: Bloat-free Immediate Mode Graphical User interface for C++ with minimal dependencies (https://github.com/ocornut/imgui))
#
# IMPORTED Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the :prop_tgt:`IMPORTED` target ``Imgui``,
# if Imgui has been found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ::
#
#   IMGUI_INCLUDE_DIRS - include directories for Imgui
#   IMGUI_SOURCES - imgui sources that build Imgui target
#   IMGUI_HEADERS - imgui headers that build Imgui target
#   Imgui_FOUND - true if Imgui has been found and can be used

if (Imgui_FOUND)
	return()
endif()

include(FindPackageHandleStandardArgs)

set(IMGUI_INCLUDE_DIRS "${ENGINE_ROOT_DIR}/ThirdParty/Imgui")
file(GLOB IMGUI_SOURCES "${ENGINE_ROOT_DIR}/ThirdParty/Imgui/*.cpp")
file(GLOB IMGUI_HEADERS "${ENGINE_ROOT_DIR}/ThirdParty/Imgui/*.h")

# avoid building target twice when Imgui is used by multiple projects: Engine and RenderingDevice
if (NOT TARGET Imgui)
	add_library(Imgui STATIC ${IMGUI_SOURCES} ${IMGUI_HEADERS})
endif()

find_package_handle_standard_args(Imgui IMGUI_SOURCES IMGUI_HEADERS IMGUI_INCLUDE_DIRS)