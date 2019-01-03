set(BOX2D_USE_SHARED OFF)

option(BOX2D_INSTALL "Install Box2D libs, includes, and CMake scripts" OFF)
option(BOX2D_INSTALL_DOC "Install Box2D documentation" OFF)
option(BOX2D_BUILD_EXAMPLES "Build Box2D examples" OFF)

if (${BOX2D_USE_SHARED})
  set(BOX2D_LIBRARIES Box2D_shared PARENT_SCOPE)
  option(BOX2D_BUILD_SHARED "Build Box2D shared libraries" ON)
  option(BOX2D_BUILD_STATIC "Build Box2D static libraries" OFF)
else()
  set(BOX2D_LIBRARIES Box2D PARENT_SCOPE)
  option(BOX2D_BUILD_SHARED "Build Box2D shared libraries" OFF)
  option(BOX2D_BUILD_STATIC "Build Box2D static libraries" ON)
endif()

set(BOX2D_VERSION 2.3.1)
set(LIB_INSTALL_DIR lib${LIB_SUFFIX})

# The Box2D library.
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Box2D/Box2D/Box2D)
set_target_properties(Box2D PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/Box2D/Box2D")
set_target_properties (Box2D PROPERTIES FOLDER "ThirdParty" )