# FindBullet
# --------
#
# Find the Bullet Physics Library (Bullet)
#
# IMPORTED Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the :prop_tgt:`IMPORTED` target ``Bullet::Bullet``,
# if Bullet has been found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ::
#
#   Bullet_INCLUDE_DIRS - include directories for Bullet
#   Bullet_LIBRARIES - libraries to link against Bullet
#   Bullet_FOUND - true if Bullet has been found and can be used

if (Bullet_FOUND)
	return()
endif()

include(FindPackageHandleStandardArgs)

if (WIN32)
	set(Bullet_ROOT_DIR "${ENGINE_ROOT_DIR}/ThirdParty/Bullet" CACHE PATH "Bullet root directory")

	find_path(Bullet_INCLUDE_DIR NAMES "btBulletCollisionCommon.h" "btBulletDynamicsCommon.h" PATHS "${Bullet_ROOT_DIR}/Src")
	find_library(BulletCollision_LIBRARY NAMES BulletCollision  HINTS "${Bullet_ROOT_DIR}/lib/Release/x64")
	find_library(BulletDynamics_LIBRARY NAMES BulletDynamics  HINTS "${Bullet_ROOT_DIR}/lib/Release/x64")
	find_library(LinearMath_LIBRARY NAMES LinearMath  HINTS "${Bullet_ROOT_DIR}/lib/Release/x64")
	find_library(Bullet3Collision_LIBRARY NAMES Bullet3Collision  HINTS "${Bullet_ROOT_DIR}/lib/Release/x64")
	find_library(Bullet3Common_LIBRARY NAMES Bullet3Common  HINTS "${Bullet_ROOT_DIR}/lib/Release/x64")
	find_library(Bullet3Dynamics_LIBRARY NAMES Bullet3Dynamics  HINTS "${Bullet_ROOT_DIR}/lib/Release/x64")
	find_library(Bullet3Geometry_LIBRARY NAMES Bullet3Geometry HINTS "${Bullet_ROOT_DIR}/lib/Release/x64")
else()
	find_package(Bullet CONFIG QUIET) #note(vuko): frequently faulty, so try to use it just for `Bullet_ROOT_DIR'
	unset(Bullet_DIR CACHE)
	unset(Bullet_INCLUDE_DIRS)

	find_path(Bullet_INCLUDE_DIR  NAMES btBulletCollisionCommon.h btBulletDynamicsCommon.h  HINTS ${Bullet_ROOT_DIR}/Src)
	find_library(BulletCollision_LIBRARY NAMES BulletCollision  HINTS ${Bullet_ROOT_DIR}/lib/Release/x64)
	find_library(BulletDynamics_LIBRARY NAMES BulletDynamics  HINTS ${Bullet_ROOT_DIR}/lib/Release/x64)
	find_library(LinearMath_LIBRARY NAMES LinearMath  HINTS ${Bullet_ROOT_DIR}/lib/Release/x64)
	find_library(Bullet3Collision_LIBRARY NAMES Bullet3Collision  HINTS ${Bullet_ROOT_DIR}/lib/Release/x64)
	find_library(Bullet3Common_LIBRARY NAMES Bullet3Common  HINTS ${Bullet_ROOT_DIR}/lib/Release/x64)
	find_library(Bullet3Dynamics_LIBRARY NAMES Bullet3Dynamics  HINTS ${Bullet_ROOT_DIR}/lib/Release/x64)
	find_library(Bullet3Geometry_LIBRARY NAMES Bullet3Geometry HINTS ${Bullet_ROOT_DIR}/lib/Release/x64)
endif()

set(Bullet_INCLUDE_DIRS "${Bullet_INCLUDE_DIR}")
set(Bullet_LIBRARIES    "${BulletCollision_LIBRARY}" "${BulletDynamics_LIBRARY}" "${LinearMath_LIBRARY}" "${Bullet3Collision_LIBRARY}" "${Bullet3Common_LIBRARY}" "${Bullet3Dynamics_LIBRARY}" "${Bullet3Geometry_LIBRARY}")
mark_as_advanced(Bullet_INCLUDE_DIR BulletCollision_LIBRARY BulletDynamics_LIBRARY LinearMath_LIBRARY Bullet3Collision_LIBRARY Bullet3Common_LIBRARY Bullet3Dynamics_LIBRARY Bullet3Geometry_LIBRARY)

find_package_handle_standard_args(Bullet  REQUIRED_VARS Bullet_LIBRARIES Bullet_INCLUDE_DIRS  VERSION_VAR)

if (NOT TARGET Bullet::Bullet)
    add_library(Bullet::Bullet INTERFACE IMPORTED)
    set_target_properties(Bullet::Bullet PROPERTIES  INTERFACE_INCLUDE_DIRECTORIES "${Bullet_INCLUDE_DIRS}"  INTERFACE_LINK_LIBRARIES "${Bullet_LIBRARIES}")
endif()