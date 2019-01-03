set(VORBIS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vorbis)
set(VORBIS_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/vorbis)

set(BUILD_SHARED_LIBS OFF)
set(BUILD_FRAMEWORK OFF)

# Extract project version from configure.ac
file(READ ${VORBIS_SOURCE_DIR}/configure.ac CONFIGURE_AC_CONTENTS)
string(REGEX MATCH "AC_INIT\\(\\[libvorbis\\],\\[([0-9]*).([0-9]*).([0-9]*)" DUMMY ${CONFIGURE_AC_CONTENTS})
set(PROJECT_VERSION_MAJOR ${CMAKE_MATCH_1})
set(PROJECT_VERSION_MINOR ${CMAKE_MATCH_2})
set(PROJECT_VERSION_PATCH ${CMAKE_MATCH_3})
set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

# Helper function to get version-info
function(get_version_info result current_var_name age_var_name revision_var_name)
    string(REGEX MATCH "${current_var_name}=([0-9]*)" DUMMY ${CONFIGURE_AC_CONTENTS})
    set(VERSION_INFO_CURRENT ${CMAKE_MATCH_1})

    string(REGEX MATCH "${age_var_name}=([0-9]*)" DUMMY ${CONFIGURE_AC_CONTENTS})
    set(VERSION_INFO_AGE ${CMAKE_MATCH_1})

    string(REGEX MATCH "${revision_var_name}=([0-9]*)" DUMMY ${CONFIGURE_AC_CONTENTS})
    set(VERSION_INFO_REVISION ${CMAKE_MATCH_1})

    math(EXPR VERSION_INFO_CURRENT_MINUS_AGE "${VERSION_INFO_CURRENT} - ${VERSION_INFO_AGE}")

    set(${result} "${VERSION_INFO_CURRENT_MINUS_AGE}.${VERSION_INFO_AGE}.${VERSION_INFO_REVISION}" PARENT_SCOPE)
endfunction()

# Find ogg dependency
set(OGG_INCLUDE_DIRS ${OGG_SOURCE_DIR}/include)
set(OGG_LIBRARIES ogg)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OGG DEFAULT_MSG OGG_INCLUDE_DIRS OGG_LIBRARIES)

add_subdirectory(${VORBIS_SOURCE_DIR}/lib)
set_target_properties(vorbis PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${VORBIS_SOURCE_DIR}/include")

set_target_properties (vorbis PROPERTIES FOLDER "ThirdParty/vorbis" )
set_target_properties (vorbisenc PROPERTIES FOLDER "ThirdParty/vorbis" )
set_target_properties (vorbisfile PROPERTIES FOLDER "ThirdParty/vorbis" )