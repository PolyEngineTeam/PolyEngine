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