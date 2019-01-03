set(FT2_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/freetype2)
set(FT2_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/freetype2)

# These options mean "require x and complain if not found". They'll get
# optionally found anyway. Use `-DCMAKE_DISABLE_FIND_PACKAGE_x=TRUE` to disable
# searching for a packge entirely (x is the CMake package name, so "BZip2"
# instead of "BZIP2").
option(FT_WITH_ZLIB "Use system zlib instead of internal library." OFF)
option(FT_WITH_BZIP2 "Support bzip2 compressed fonts." OFF)
option(FT_WITH_PNG "Support PNG compressed OpenType embedded bitmaps." OFF)
option(FT_WITH_HARFBUZZ "Improve auto-hinting of OpenType fonts." OFF)

# Add local cmake modules
list(APPEND CMAKE_MODULE_PATH ${FT2_SRC_DIR}/builds/cmake)

set(BUILD_SHARED_LIBS ON)
if (BUILD_FRAMEWORK)
  if (NOT "${CMAKE_GENERATOR}" STREQUAL "Xcode")
    message(FATAL_ERROR
      "You should use Xcode generator with BUILD_FRAMEWORK enabled")
  endif ()
  set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD_32_64_BIT)")
  set(BUILD_SHARED_LIBS ON)
endif ()


# Find dependencies
if (FT_WITH_HARFBUZZ)
  find_package(HarfBuzz 1.3.0 REQUIRED)
else ()
  find_package(HarfBuzz 1.3.0)
endif ()

if (FT_WITH_PNG)
  find_package(PNG REQUIRED)
else ()
  find_package(PNG)
endif ()

if (FT_WITH_ZLIB)
  find_package(ZLIB REQUIRED)
else ()
  find_package(ZLIB)
endif ()

if (FT_WITH_BZIP2)
  find_package(BZip2 REQUIRED)
else ()
  find_package(BZip2)
endif ()

# Create the configuration file
if (UNIX)
  check_include_file("unistd.h" HAVE_UNISTD_H)
  check_include_file("fcntl.h" HAVE_FCNTL_H)
  check_include_file("stdint.h" HAVE_STDINT_H)

  file(READ "${FT2_SRC_DIR}/builds/unix/ftconfig.in"
    FTCONFIG_H)
  if (HAVE_UNISTD_H)
    string(REGEX REPLACE
      "#undef +(HAVE_UNISTD_H)" "#define \\1 1"
      FTCONFIG_H "${FTCONFIG_H}")
  endif ()
  if (HAVE_FCNTL_H)
    string(REGEX REPLACE
      "#undef +(HAVE_FCNTL_H)" "#define \\1 1"
      FTCONFIG_H "${FTCONFIG_H}")
  endif ()
  if (HAVE_STDINT_H)
    string(REGEX REPLACE
      "#undef +(HAVE_STDINT_H)" "#define \\1 1"
      FTCONFIG_H "${FTCONFIG_H}")
  endif ()
  string(REPLACE "/undef " "#undef "
    FTCONFIG_H "${FTCONFIG_H}")
  file(WRITE "${FT2_BIN_DIR}/include/freetype/config/ftconfig.h"
    "${FTCONFIG_H}")
endif ()


# Create the options file
file(READ "${FT2_SRC_DIR}/include/freetype/config/ftoption.h"
  FTOPTION_H)
if (ZLIB_FOUND)
  string(REGEX REPLACE
    "/\\* +(#define +FT_CONFIG_OPTION_SYSTEM_ZLIB) +\\*/" "\\1"
    FTOPTION_H "${FTOPTION_H}")
endif ()
if (BZIP2_FOUND)
  string(REGEX REPLACE
    "/\\* +(#define +FT_CONFIG_OPTION_USE_BZIP2) +\\*/" "\\1"
    FTOPTION_H "${FTOPTION_H}")
endif ()
if (PNG_FOUND)
  string(REGEX REPLACE
    "/\\* +(#define +FT_CONFIG_OPTION_USE_PNG) +\\*/" "\\1"
    FTOPTION_H "${FTOPTION_H}")
endif ()
if (HARFBUZZ_FOUND)
  string(REGEX REPLACE
    "/\\* +(#define +FT_CONFIG_OPTION_USE_HARFBUZZ) +\\*/" "\\1"
    FTOPTION_H "${FTOPTION_H}")
endif ()
file(WRITE "${FT2_BIN_DIR}/include/freetype/config/ftoption.h"
  "${FTOPTION_H}")

file(GLOB PUBLIC_HEADERS "${FT2_SRC_DIR}/include/ft2build.h" "${FT2_SRC_DIR}/include/freetype/*.h")
file(GLOB PUBLIC_CONFIG_HEADERS "${FT2_SRC_DIR}/include/freetype/config/*.h")
file(GLOB PRIVATE_HEADERS "${FT2_SRC_DIR}/include/freetype/internal/*.h")


set(BASE_SRCS
  ${FT2_SRC_DIR}/src/autofit/autofit.c
  ${FT2_SRC_DIR}/src/base/ftbase.c
  ${FT2_SRC_DIR}/src/base/ftbbox.c
  ${FT2_SRC_DIR}/src/base/ftbdf.c
  ${FT2_SRC_DIR}/src/base/ftbitmap.c
  ${FT2_SRC_DIR}/src/base/ftcid.c
  ${FT2_SRC_DIR}/src/base/ftfstype.c
  ${FT2_SRC_DIR}/src/base/ftgasp.c
  ${FT2_SRC_DIR}/src/base/ftglyph.c
  ${FT2_SRC_DIR}/src/base/ftgxval.c
  ${FT2_SRC_DIR}/src/base/ftinit.c
  ${FT2_SRC_DIR}/src/base/ftmm.c
  ${FT2_SRC_DIR}/src/base/ftotval.c
  ${FT2_SRC_DIR}/src/base/ftpatent.c
  ${FT2_SRC_DIR}/src/base/ftpfr.c
  ${FT2_SRC_DIR}/src/base/ftstroke.c
  ${FT2_SRC_DIR}/src/base/ftsynth.c
  ${FT2_SRC_DIR}/src/base/ftsystem.c
  ${FT2_SRC_DIR}/src/base/fttype1.c
  ${FT2_SRC_DIR}/src/base/ftwinfnt.c
  ${FT2_SRC_DIR}/src/bdf/bdf.c
  ${FT2_SRC_DIR}/src/bzip2/ftbzip2.c
  ${FT2_SRC_DIR}/src/cache/ftcache.c
  ${FT2_SRC_DIR}/src/cff/cff.c
  ${FT2_SRC_DIR}/src/cid/type1cid.c
  ${FT2_SRC_DIR}/src/gzip/ftgzip.c
  ${FT2_SRC_DIR}/src/lzw/ftlzw.c
  ${FT2_SRC_DIR}/src/pcf/pcf.c
  ${FT2_SRC_DIR}/src/pfr/pfr.c
  ${FT2_SRC_DIR}/src/psaux/psaux.c
  ${FT2_SRC_DIR}/src/pshinter/pshinter.c
  ${FT2_SRC_DIR}/src/psnames/psnames.c
  ${FT2_SRC_DIR}/src/raster/raster.c
  ${FT2_SRC_DIR}/src/sfnt/sfnt.c
  ${FT2_SRC_DIR}/src/smooth/smooth.c
  ${FT2_SRC_DIR}/src/truetype/truetype.c
  ${FT2_SRC_DIR}/src/type1/type1.c
  ${FT2_SRC_DIR}/src/type42/type42.c
  ${FT2_SRC_DIR}/src/winfonts/winfnt.c
)

if (WIN32)
  enable_language(RC)
  list(APPEND BASE_SRCS ${FT2_SRC_DIR}/builds/windows/ftdebug.c
                        ${FT2_SRC_DIR}/src/base/ftver.rc)
elseif (WINCE)
  list(APPEND BASE_SRCS ${FT2_SRC_DIR}/builds/wince/ftdebug.c)
else ()
  list(APPEND BASE_SRCS ${FT2_SRC_DIR}/src/base/ftdebug.c)
endif ()

if (BUILD_FRAMEWORK)
  list(APPEND BASE_SRCS ${FT2_SRC_DIR}/builds/mac/freetype-Info.plist)
endif ()

add_library(freetype
  ${PUBLIC_HEADERS}
  ${PUBLIC_CONFIG_HEADERS}
  ${PRIVATE_HEADERS}
  ${BASE_SRCS}
)

set_target_properties(freetype PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${FT2_SRC_DIR}/include")

set_target_properties(
  freetype PROPERTIES
    C_VISIBILITY_PRESET hidden)

target_compile_definitions(
  freetype PRIVATE FT2_BUILD_LIBRARY)

if (WIN32)
  target_compile_definitions(
    freetype PRIVATE _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_WARNINGS)
endif ()

#if (BUILD_SHARED_LIBS)
#  set_target_properties(freetype PROPERTIES )
#endif ()

target_include_directories(
  freetype BEFORE  # Pick up ftconfig.h and ftoption.h generated above.
    PRIVATE "${FT2_BIN_DIR}/include")

target_include_directories(
  freetype
    PRIVATE "${FT2_SRC_DIR}/include")

target_include_directories(
  freetype
    PUBLIC $<INSTALL_INTERFACE:include/freetype2>)

if (BUILD_FRAMEWORK)
  set_property(SOURCE ${PUBLIC_CONFIG_HEADERS}
    PROPERTY MACOSX_PACKAGE_LOCATION Headers/config
  )
  set_target_properties(freetype PROPERTIES
    FRAMEWORK TRUE
    MACOSX_FRAMEWORK_INFO_PLIST builds/mac/freetype-Info.plist
    PUBLIC_HEADER "${PUBLIC_HEADERS}"
    XCODE_ATTRIBUTE_INSTALL_PATH "@rpath"
  )
endif ()


set(PKG_CONFIG_REQUIRED_PRIVATE "")

if (ZLIB_FOUND)
  target_link_libraries(freetype PRIVATE ${ZLIB_LIBRARIES})
  target_include_directories(freetype PRIVATE ${ZLIB_INCLUDE_DIRS})
  list(APPEND PKG_CONFIG_REQUIRED_PRIVATE zlib)
endif ()
if (BZIP2_FOUND)
  target_link_libraries(freetype PRIVATE ${BZIP2_LIBRARIES})
  target_include_directories(freetype PRIVATE ${BZIP2_INCLUDE_DIR}) # not BZIP2_INCLUDE_DIRS
  list(APPEND PKG_CONFIG_REQUIRED_PRIVATE bzip2)
endif ()
if (PNG_FOUND)
  target_link_libraries(freetype PRIVATE ${PNG_LIBRARIES})
  target_compile_definitions(freetype PRIVATE ${PNG_DEFINITIONS})
  target_include_directories(freetype PRIVATE ${PNG_INCLUDE_DIRS})
  list(APPEND PKG_CONFIG_REQUIRED_PRIVATE libpng)
endif ()
if (HARFBUZZ_FOUND)
  target_link_libraries(freetype PRIVATE ${HARFBUZZ_LIBRARIES})
  target_include_directories(freetype PRIVATE ${HARFBUZZ_INCLUDE_DIRS})
  list(APPEND PKG_CONFIG_REQUIRED_PRIVATE harfbuzz)
endif ()

set_target_properties (freetype PROPERTIES FOLDER "ThirdParty" )