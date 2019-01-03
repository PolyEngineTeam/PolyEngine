set(OGG_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ogg)
set(OGG_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/ogg)

# Build options
set(BUILD_SHARED_LIBS OFF)
set(BUILD_FRAMEWORK OFF)

# Extract project version from configure.ac
file(READ ${OGG_SOURCE_DIR}/configure.ac CONFIGURE_AC_CONTENTS)
string(REGEX MATCH "AC_INIT\\(\\[libogg\\],\\[([0-9]*).([0-9]*).([0-9]*)" DUMMY ${CONFIGURE_AC_CONTENTS})
set(PROJECT_VERSION_MAJOR ${CMAKE_MATCH_1})
set(PROJECT_VERSION_MINOR ${CMAKE_MATCH_2})
set(PROJECT_VERSION_PATCH ${CMAKE_MATCH_3})
set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

# Configure config_type.h
check_include_files(inttypes.h INCLUDE_INTTYPES_H)
check_include_files(stdint.h INCLUDE_STDINT_H)
check_include_files(sys/types.h INCLUDE_SYS_TYPES_H)

set(SIZE16 int16_t)
set(USIZE16 uint16_t)
set(SIZE32 int32_t)
set(USIZE32 uint32_t)
set(SIZE64 int64_t)

configure_file(${OGG_SOURCE_DIR}/include/ogg/config_types.h.in ${OGG_BIN_DIR}/include/ogg/config_types.h @ONLY)

set(OGG_HEADERS
    ${OGG_BIN_DIR}/include/ogg/config_types.h
    ${OGG_SOURCE_DIR}/include/ogg/ogg.h
    ${OGG_SOURCE_DIR}/include/ogg/os_types.h
)

set(OGG_SOURCES
    ${OGG_SOURCE_DIR}/src/bitwise.c
    ${OGG_SOURCE_DIR}/src/framing.c
)

if(MSVC)
    list(APPEND OGG_SOURCES ${OGG_SOURCE_DIR}/win32/ogg.def)
endif()

if(BUILD_FRAMEWORK)
    set(BUILD_SHARED_LIBS TRUE)
endif()

add_library(ogg ${OGG_HEADERS} ${OGG_SOURCES})
target_include_directories(ogg PUBLIC ${OGG_SOURCE_DIR}/include ${OGG_BIN_DIR}/include)

set_target_properties(
    ogg PROPERTIES
    PUBLIC_HEADER "${OGG_HEADERS}"
    INTERFACE_INCLUDE_DIRECTORIES "${OGG_SOURCE_DIR}/include;${OGG_BIN_DIR}/include"
)
set_target_properties (ogg PROPERTIES FOLDER "ThirdParty" )
