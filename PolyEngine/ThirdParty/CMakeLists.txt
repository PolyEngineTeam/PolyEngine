## Thirparty libraries

###
# Assimp
###

# Configuration
set( ASSIMP_LIB_INSTALL_DIR "lib" CACHE STRING
  "Path the built library files are installed to." )
set( ASSIMP_INCLUDE_INSTALL_DIR "include" CACHE STRING
  "Path the header files are installed to." )
set( ASSIMP_BIN_INSTALL_DIR "bin" CACHE STRING
  "Path the tool executables are installed to." )

# Prerequisites
if(NOT GIT_COMMIT_HASH)
  set(GIT_COMMIT_HASH 0)
endif(NOT GIT_COMMIT_HASH)

# Config files
configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/assimp/revision.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/assimp/revision.h
)
configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/assimp/include/assimp/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/assimp/include/assimp/config.h
)

# zlib
add_subdirectory(assimp/contrib/zlib)
set(ZLIB_FOUND 1)
set(ZLIB_LIBRARIES zlibstatic)
set(ZLIB_INCLUDE_DIR
  ${CMAKE_CURRENT_LIST_DIR}/assimp/contrib/zlib
  ${CMAKE_CURRENT_BINARY_DIR}/assimp/contrib/zlib
  )

set(ASSIMP_INCLUDE_DIR 
  ${CMAKE_CURRENT_LIST_DIR}/assimp/
  ${CMAKE_CURRENT_LIST_DIR}/assimp/include
  ${CMAKE_CURRENT_BINARY_DIR}/assimp/
  ${CMAKE_CURRENT_BINARY_DIR}/assimp/include
)
include_directories(${ASSIMP_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR})

# irrXML
add_subdirectory(assimp/contrib/irrXML)

# Finally assimp
add_definitions( -DASSIMP_BUILD_NO_C4D_IMPORTER )
add_subdirectory(assimp/code)

# Export assimp include to the solution
set(ASSIMP_INCLUDE_DIR ${ASSIMP_INCLUDE_DIR} PARENT_SCOPE)

###
# ImGui
###
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Imgui)
set(IMGUI_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/Imgui PARENT_SCOPE)

###
# Box2D
###
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
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Box2D/Box2D/Box2D)
set(BOX2D_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/Box2D/Box2D PARENT_SCOPE)

###
# Bullet3
###

option(BUILD_BULLET3 "Set when you want to build Bullet 3" OFF)
set(BUILD_SHARED_LIBS OFF)

if(BUILD_BULLET3)
  set(BULLET3_LIBRARIES 
    Bullet3OpenCL_clew
    Bullet2FileLoader
    Bullet3Dynamics
    Bullet3Collision
    Bullet3Geometry
  )
endif(BUILD_BULLET3)

set(BULLET_LIBRARIES 
  
  ${BULLET3_LIBRARIES}
  BulletInverseDynamics
  BulletSoftBody
  BulletCollision
  BulletDynamics
  LinearMath
  Bullet3Common

  PARENT_SCOPE)

set(BULLET_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/bullet3/src PARENT_SCOPE)

# The Bullet library.
file(STRINGS "${CMAKE_CURRENT_LIST_DIR}/bullet3/VERSION" BULLET_VERSION)
set(BULLET_PHYSICS_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/bullet3/)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/bullet3/src)

###
# Catch include
###
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Catch2/single_include/catch2)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/Catch2/contrib)
include(ParseAndAddCatchTests)
