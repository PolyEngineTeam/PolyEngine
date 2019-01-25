
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
  Bullet3Common)

  set(BULLET_LIBRARIES ${BULLET_LIBRARIES} PARENT_SCOPE)

set(BULLET_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/bullet3/src PARENT_SCOPE)

# The Bullet library.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/bullet3/VERSION" BULLET_VERSION)
set(BULLET_PHYSICS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bullet3/)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/bullet3/src)

foreach(BulletTarget ${BULLET_LIBRARIES})
  set_target_properties (${BulletTarget} PROPERTIES FOLDER "ThirdParty/Bullet" )
endforeach()