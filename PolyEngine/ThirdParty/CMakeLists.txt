## Thirparty libraries



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
)
include_directories()
add_subdirectory(assimp/contrib/irrXML)
add_definitions( -DASSIMP_BUILD_NO_C4D_IMPORTER )
add_subdirectory(assimp/code)