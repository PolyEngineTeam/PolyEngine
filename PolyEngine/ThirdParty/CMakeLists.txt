## Thirparty libraries

# Cache these to allow the user to override them manually.
SET( ASSIMP_LIB_INSTALL_DIR "lib" CACHE STRING
  "Path the built library files are installed to." )
SET( ASSIMP_INCLUDE_INSTALL_DIR "include" CACHE STRING
  "Path the header files are installed to." )
SET( ASSIMP_BIN_INSTALL_DIR "bin" CACHE STRING
  "Path the tool executables are installed to." )

IF(NOT GIT_COMMIT_HASH)
  SET(GIT_COMMIT_HASH 0)
ENDIF(NOT GIT_COMMIT_HASH)

CONFIGURE_FILE(
    ${CMAKE_CURRENT_LIST_DIR}/assimp/revision.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/assimp/revision.h
)

CONFIGURE_FILE(
    ${CMAKE_CURRENT_LIST_DIR}/assimp/include/assimp/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/assimp/include/assimp/config.h
)

# zlib
add_subdirectory(assimp/contrib/zlib)
SET(ZLIB_FOUND 1)
SET(ZLIB_LIBRARIES zlibstatic)
SET(ZLIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assimp/contrib/zlib ${CMAKE_CURRENT_BINARY_DIR}/assimp/contrib/zlib)

# assimp
include_directories(
    assimp/
    assimp/include
    ${ZLIB_INCLUDE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/assimp/
    ${CMAKE_CURRENT_BINARY_DIR}/assimp/include
)
include_directories()
add_subdirectory(assimp/contrib/irrXML)
add_definitions( -DASSIMP_BUILD_NO_C4D_IMPORTER )

add_subdirectory(assimp/code)