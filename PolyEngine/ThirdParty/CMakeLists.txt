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
set(BUILD_SHARED_LIBS ON)

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

###
# freetype2
###
set(FT2_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/freetype2)
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

set(FREETYPE_INCLUDE_DIRS ${FT2_SRC_DIR}/include PARENT_SCOPE)

set_target_properties(
  freetype PROPERTIES
    C_VISIBILITY_PRESET hidden)

target_compile_definitions(
  freetype PRIVATE FT2_BUILD_LIBRARY)

if (WIN32)
  target_compile_definitions(
    freetype PRIVATE _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_WARNINGS)
endif ()

if (BUILD_SHARED_LIBS)
  set_target_properties(freetype PROPERTIES
    VERSION ${LIBRARY_VERSION}
    SOVERSION ${LIBRARY_SOVERSION})
endif ()

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

###
# ImGui
###
file(GLOB IMGUI_SOURCES ${CMAKE_CURRENT_LIST_DIR}/imgui/*.cpp)
file(GLOB IMGUI_HEADERS  ${CMAKE_CURRENT_LIST_DIR}/imgui/*.h)
add_library(Imgui STATIC ${IMGUI_SOURCES} ${IMGUI_HEADERS})
# GCC complains during linking of shared libraries that use this target (Engine, RenderingDevice)
set_property(TARGET Imgui PROPERTY POSITION_INDEPENDENT_CODE ON)
set(IMGUI_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/imgui PARENT_SCOPE)

###
# SDL2
###
set(SDL2_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/SDL2)
set(SDL2_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/SDL2)

include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckIncludeFiles)
include(CheckIncludeFile)
include(CheckSymbolExists)
include(CheckCSourceCompiles)
include(CheckCSourceRuns)
include(CheckCCompilerFlag)
include(CheckTypeSize)
include(CheckStructHasMember)
include(CMakeDependentOption)
include(FindPkgConfig)
include(GNUInstallDirs)
set(CMAKE_MODULE_PATH "${SDL2_SOURCE_DIR}/cmake")
include(${SDL2_SOURCE_DIR}/cmake/macros.cmake)
include(${SDL2_SOURCE_DIR}/cmake/sdlchecks.cmake)

# Set defaults preventing destination file conflicts
set(SDL_CMAKE_DEBUG_POSTFIX ""
    CACHE STRING "Name suffix for debug builds")

mark_as_advanced(CMAKE_IMPORT_LIBRARY_SUFFIX SDL_CMAKE_DEBUG_POSTFIX)

set(SDL_MAJOR_VERSION 2)
set(SDL_MINOR_VERSION 0)
set(SDL_MICRO_VERSION 9)
set(SDL_INTERFACE_AGE 0)
set(SDL_BINARY_AGE 9)
set(SDL_VERSION "${SDL_MAJOR_VERSION}.${SDL_MINOR_VERSION}.${SDL_MICRO_VERSION}")
# the following should match the versions in Xcode project file:
set(DYLIB_CURRENT_VERSION 10.0.0)
set(DYLIB_COMPATIBILITY_VERSION 1.0.0)

# Calculate a libtool-like version number
math(EXPR LT_CURRENT "${SDL_MICRO_VERSION} - ${SDL_INTERFACE_AGE}")
math(EXPR LT_AGE "${SDL_BINARY_AGE} - ${SDL_INTERFACE_AGE}")
math(EXPR LT_MAJOR "${LT_CURRENT}- ${LT_AGE}")
set(LT_REVISION "${SDL_INTERFACE_AGE}")
set(LT_RELEASE "${SDL_MAJOR_VERSION}.${SDL_MINOR_VERSION}")
set(LT_VERSION "${LT_MAJOR}.${LT_AGE}.${LT_REVISION}")

set(SIZEOF_VOIDP ${CMAKE_SIZEOF_VOID_P})
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(ARCH_64 TRUE)
  set(PROCESSOR_ARCH "x64")
else()
  set(ARCH_64 FALSE)
  set(PROCESSOR_ARCH "x86")
endif()
set(LIBNAME SDL2)
if(NOT LIBTYPE)
  set(LIBTYPE SHARED)
endif()

# Get the platform
if(WIN32)
  if(NOT WINDOWS)
    set(WINDOWS TRUE)
  endif()
elseif(UNIX AND NOT APPLE)
  if(CMAKE_SYSTEM_NAME MATCHES ".*Linux")
    set(LINUX TRUE)
  elseif(CMAKE_SYSTEM_NAME MATCHES "kFreeBSD.*")
    set(FREEBSD TRUE)
  elseif(CMAKE_SYSTEM_NAME MATCHES "kNetBSD.*|NetBSD.*")
    set(NETBSD TRUE)
  elseif(CMAKE_SYSTEM_NAME MATCHES "kOpenBSD.*|OpenBSD.*")
    set(OPENBSD TRUE)
  elseif(CMAKE_SYSTEM_NAME MATCHES ".*GNU.*")
    set(GNU TRUE)
  elseif(CMAKE_SYSTEM_NAME MATCHES ".*BSDI.*")
    set(BSDI TRUE)
  elseif(CMAKE_SYSTEM_NAME MATCHES "DragonFly.*|FreeBSD")
    set(FREEBSD TRUE)
  elseif(CMAKE_SYSTEM_NAME MATCHES "SYSV5.*")
    set(SYSV5 TRUE)
  elseif(CMAKE_SYSTEM_NAME MATCHES "Solaris.*")
    set(SOLARIS TRUE)
  elseif(CMAKE_SYSTEM_NAME MATCHES "HP-UX.*")
    set(HPUX TRUE)
  elseif(CMAKE_SYSTEM_NAME MATCHES "AIX.*")
    set(AIX TRUE)
  elseif(CMAKE_SYSTEM_NAME MATCHES "Minix.*")
    set(MINIX TRUE)
  endif()
elseif(APPLE)
  if(CMAKE_SYSTEM_NAME MATCHES ".*Darwin.*")
    set(DARWIN TRUE)
  elseif(CMAKE_SYSTEM_NAME MATCHES ".*MacOS.*")
    set(MACOSX TRUE)
  endif()
  # TODO: iOS?
elseif(CMAKE_SYSTEM_NAME MATCHES "BeOS.*")
  message_error("BeOS support has been removed as of SDL 2.0.2.")
elseif(CMAKE_SYSTEM_NAME MATCHES "Haiku.*")
  set(HAIKU TRUE)
endif()

# Don't mistake osx for unix
if(UNIX AND NOT APPLE)
  set(UNIX_SYS ON)
else()
  set(UNIX_SYS OFF)
endif()

if(UNIX OR APPLE)
  set(UNIX_OR_MAC_SYS ON)
else()
  set(UNIX_OR_MAC_SYS OFF)
endif()

if (UNIX_OR_MAC_SYS AND NOT EMSCRIPTEN) # JavaScript does not yet have threading support, so disable pthreads when building for Emscripten.
  set(SDL_PTHREADS_ENABLED_BY_DEFAULT ON)
else()
  set(SDL_PTHREADS_ENABLED_BY_DEFAULT OFF)
endif()

# Default option knobs
if(APPLE OR ARCH_64)
  if(NOT "${CMAKE_OSX_ARCHITECTURES}" MATCHES "arm")
    set(OPT_DEF_SSEMATH ON)
  endif()
endif()
if(UNIX OR MINGW OR MSYS)
  set(OPT_DEF_LIBC ON)
endif()

# Compiler info
if(CMAKE_COMPILER_IS_GNUCC)
  set(USE_GCC TRUE)
  set(OPT_DEF_ASM TRUE)
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
  set(USE_CLANG TRUE)
  set(OPT_DEF_ASM TRUE)
elseif(MSVC_VERSION GREATER 1400) # VisualStudio 8.0+
  set(OPT_DEF_ASM TRUE)
  #set(CMAKE_C_FLAGS "/ZI /WX- /
else()
  set(OPT_DEF_ASM FALSE)
endif()

if(USE_GCC OR USE_CLANG)
  set(OPT_DEF_GCC_ATOMICS ON)
endif()

# Default flags, if not set otherwise
if("$ENV{CFLAGS}" STREQUAL "")
  if(CMAKE_BUILD_TYPE STREQUAL "")
    if(USE_GCC OR USE_CLANG)
      set(CMAKE_C_FLAGS "-g -O3")
    endif()
  endif()
else()
  set(CMAKE_C_FLAGS "$ENV{CFLAGS}")
  list(APPEND EXTRA_CFLAGS "$ENV{CFLAGS}")
endif()
if(NOT ("$ENV{CFLAGS}" STREQUAL "")) # Hackish, but does the trick on Win32
  list(APPEND EXTRA_LDFLAGS "$ENV{LDFLAGS}")
endif()

if(MSVC)
  option(FORCE_STATIC_VCRT "Force /MT for static VC runtimes" OFF)
  if(FORCE_STATIC_VCRT)
    foreach(flag_var
        CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
      if(${flag_var} MATCHES "/MD")
        string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
      endif()
    endforeach()
  endif()

  # Make sure /RTC1 is disabled, otherwise it will use functions from the CRT
  foreach(flag_var
      CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
      CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
    string(REGEX REPLACE "/RTC(su|[1su])" "" ${flag_var} "${${flag_var}}")
  endforeach(flag_var)
endif()

# Those are used for pkg-config and friends, so that the SDL2.pc, sdl2-config,
# etc. are created correctly.
set(SDL_LIBS "-lSDL2")
set(SDL_CFLAGS "")

# When building shared lib for Windows with MinGW,
# avoid the DLL having a "lib" prefix
if(WINDOWS)
  set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif()

# Emscripten toolchain has a nonempty default value for this, and the checks
# in this file need to change that, so remember the original value, and
# restore back to that afterwards. For check_function_exists() to work in
# Emscripten, this value must be at its default value.
set(ORIG_CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})

if(CYGWIN)
  # We build SDL on cygwin without the UNIX emulation layer
  include_directories("-I/usr/include/mingw")
  set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -mno-cygwin")
  check_c_source_compiles("int main(int argc, char **argv) {}"
    HAVE_GCC_NO_CYGWIN)
  set(CMAKE_REQUIRED_FLAGS ${ORIG_CMAKE_REQUIRED_FLAGS})
  if(HAVE_GCC_NO_CYGWIN)
    list(APPEND EXTRA_LDFLAGS "-mno-cygwin")
    list(APPEND SDL_LIBS "-mno-cygwin")
  endif()
  set(SDL_CFLAGS "${SDL_CFLAGS} -I/usr/include/mingw")
endif()

add_definitions(-DUSING_GENERATED_CONFIG_H)
# General includes
include_directories(${SDL2_BINARY_DIR}/include ${SDL2_SOURCE_DIR}/include)
if(USE_GCC OR USE_CLANG)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -idirafter ${SDL2_SOURCE_DIR}/src/video/khronos")
else()
  include_directories(${SDL2_SOURCE_DIR}/src/video/khronos)
endif()

# All these ENABLED_BY_DEFAULT vars will default to ON if not specified, so
#  you only need to have a platform override them if they are disabling.
set(OPT_DEF_ASM TRUE)
if(EMSCRIPTEN)
  # Set up default values for the currently supported set of subsystems:
  # Emscripten/Javascript does not have assembly support, a dynamic library
  # loading architecture, low-level CPU inspection or multithreading.
  set(OPT_DEF_ASM FALSE)
  set(SDL_SHARED_ENABLED_BY_DEFAULT OFF)
  set(SDL_ATOMIC_ENABLED_BY_DEFAULT OFF)
  set(SDL_THREADS_ENABLED_BY_DEFAULT OFF)
  set(SDL_LOADSO_ENABLED_BY_DEFAULT OFF)
  set(SDL_CPUINFO_ENABLED_BY_DEFAULT OFF)
  set(SDL_DLOPEN_ENABLED_BY_DEFAULT OFF)
endif()

if (NOT DEFINED SDL_SHARED_ENABLED_BY_DEFAULT)
    set(SDL_SHARED_ENABLED_BY_DEFAULT ON)
endif()

set(SDL_SUBSYSTEMS
    Atomic Audio Video Render Events Joystick Haptic Power Threads Timers
    File Loadso CPUinfo Filesystem Dlopen Sensor)
foreach(_SUB ${SDL_SUBSYSTEMS})
  string(TOUPPER ${_SUB} _OPT)
  if (NOT DEFINED SDL_${_OPT}_ENABLED_BY_DEFAULT)
    set(SDL_${_OPT}_ENABLED_BY_DEFAULT ON)
  endif()
  option(SDL_${_OPT} "Enable the ${_SUB} subsystem" ${SDL_${_OPT}_ENABLED_BY_DEFAULT})
endforeach()

option_string(ASSERTIONS "Enable internal sanity checks (auto/disabled/release/enabled/paranoid)" "auto")
#set_option(DEPENDENCY_TRACKING "Use gcc -MMD -MT dependency tracking" ON)
set_option(LIBC                "Use the system C library" ${OPT_DEF_LIBC})
set_option(GCC_ATOMICS         "Use gcc builtin atomics" ${OPT_DEF_GCC_ATOMICS})
set_option(ASSEMBLY            "Enable assembly routines" ${OPT_DEF_ASM})
set_option(SSEMATH             "Allow GCC to use SSE floating point math" ${OPT_DEF_SSEMATH})
set_option(MMX                 "Use MMX assembly routines" ${OPT_DEF_ASM})
set_option(3DNOW               "Use 3Dnow! MMX assembly routines" ${OPT_DEF_ASM})
set_option(SSE                 "Use SSE assembly routines" ${OPT_DEF_ASM})
set_option(SSE2                "Use SSE2 assembly routines" ${OPT_DEF_SSEMATH})
set_option(SSE3                "Use SSE3 assembly routines" ${OPT_DEF_SSEMATH})
set_option(ALTIVEC             "Use Altivec assembly routines" ${OPT_DEF_ASM})
set_option(DISKAUDIO           "Support the disk writer audio driver" ON)
set_option(DUMMYAUDIO          "Support the dummy audio driver" ON)
set_option(VIDEO_DIRECTFB      "Use DirectFB video driver" OFF)
dep_option(DIRECTFB_SHARED     "Dynamically load directfb support" ON "VIDEO_DIRECTFB" OFF)
set_option(VIDEO_DUMMY         "Use dummy video driver" ON)
set_option(VIDEO_OPENGL        "Include OpenGL support" ON)
set_option(VIDEO_OPENGLES      "Include OpenGL ES support" ON)
set_option(PTHREADS            "Use POSIX threads for multi-threading" ${SDL_PTHREADS_ENABLED_BY_DEFAULT})
dep_option(PTHREADS_SEM        "Use pthread semaphores" ON "PTHREADS" OFF)
set_option(SDL_DLOPEN          "Use dlopen for shared object loading" ${SDL_DLOPEN_ENABLED_BY_DEFAULT})
set_option(OSS                 "Support the OSS audio API" ${UNIX_SYS})
set_option(ALSA                "Support the ALSA audio API" ${UNIX_SYS})
dep_option(ALSA_SHARED         "Dynamically load ALSA audio support" ON "ALSA" OFF)
set_option(JACK                "Support the JACK audio API" ${UNIX_SYS})
dep_option(JACK_SHARED         "Dynamically load JACK audio support" ON "JACK" OFF)
set_option(ESD                 "Support the Enlightened Sound Daemon" ${UNIX_SYS})
dep_option(ESD_SHARED          "Dynamically load ESD audio support" ON "ESD" OFF)
set_option(PULSEAUDIO          "Use PulseAudio" ${UNIX_SYS})
dep_option(PULSEAUDIO_SHARED   "Dynamically load PulseAudio support" ON "PULSEAUDIO" OFF)
set_option(ARTS                "Support the Analog Real Time Synthesizer" ${UNIX_SYS})
dep_option(ARTS_SHARED         "Dynamically load aRts audio support" ON "ARTS" OFF)
set_option(NAS                 "Support the NAS audio API" ${UNIX_SYS})
set_option(NAS_SHARED          "Dynamically load NAS audio API" ${UNIX_SYS})
set_option(SNDIO               "Support the sndio audio API" ${UNIX_SYS})
set_option(FUSIONSOUND         "Use FusionSound audio driver" OFF)
dep_option(FUSIONSOUND_SHARED  "Dynamically load fusionsound audio support" ON "FUSIONSOUND" OFF)
set_option(LIBSAMPLERATE       "Use libsamplerate for audio rate conversion" ${UNIX_SYS})
dep_option(LIBSAMPLERATE_SHARED "Dynamically load libsamplerate" ON "LIBSAMPLERATE" OFF)
set_option(RPATH               "Use an rpath when linking SDL" ${UNIX_SYS})
set_option(CLOCK_GETTIME       "Use clock_gettime() instead of gettimeofday()" OFF)
set_option(INPUT_TSLIB         "Use the Touchscreen library for input" ${UNIX_SYS})
set_option(VIDEO_X11           "Use X11 video driver" ${UNIX_SYS})
set_option(VIDEO_WAYLAND       "Use Wayland video driver" ${UNIX_SYS})
dep_option(WAYLAND_SHARED      "Dynamically load Wayland support" ON "VIDEO_WAYLAND" OFF)
dep_option(VIDEO_WAYLAND_QT_TOUCH  "QtWayland server support for Wayland video driver" ON "VIDEO_WAYLAND" OFF)
set_option(VIDEO_RPI           "Use Raspberry Pi video driver" ${UNIX_SYS})
dep_option(X11_SHARED          "Dynamically load X11 support" ON "VIDEO_X11" OFF)
set(SDL_X11_OPTIONS Xcursor Xinerama XInput Xrandr Xscrnsaver XShape Xvm)
foreach(_SUB ${SDL_X11_OPTIONS})
  string(TOUPPER "VIDEO_X11_${_SUB}" _OPT)
  dep_option(${_OPT}           "Enable ${_SUB} support" ON "VIDEO_X11" OFF)
endforeach()
set_option(VIDEO_COCOA         "Use Cocoa video driver" ${APPLE})
set_option(DIRECTX             "Use DirectX for Windows audio/video" ${WINDOWS})
set_option(WASAPI              "Use the Windows WASAPI audio driver" ${WINDOWS})
set_option(RENDER_D3D          "Enable the Direct3D render driver" ${WINDOWS})
set_option(VIDEO_VIVANTE       "Use Vivante EGL video driver" ${UNIX_SYS})
dep_option(VIDEO_VULKAN        "Enable Vulkan support" ON "ANDROID OR APPLE OR LINUX OR WINDOWS" OFF)
set_option(VIDEO_KMSDRM        "Use KMS DRM video driver" ${UNIX_SYS})
dep_option(KMSDRM_SHARED       "Dynamically load KMS DRM support" ON "VIDEO_KMSDRM" OFF)

# TODO: We should (should we?) respect cmake's ${BUILD_SHARED_LIBS} flag here
# The options below are for compatibility to configure's default behaviour.
set(SDL_SHARED ${SDL_SHARED_ENABLED_BY_DEFAULT} CACHE BOOL "Build a shared version of the library")
set(SDL_STATIC OFF CACHE BOOL "Build a static version of the library")

dep_option(SDL_STATIC_PIC      "Static version of the library should be built with Position Independent Code" OFF "SDL_STATIC" OFF)
set_option(SDL_TEST            "Build the test directory" OFF)

# General source files
file(GLOB SOURCE_FILES
  ${SDL2_SOURCE_DIR}/src/*.c
  ${SDL2_SOURCE_DIR}/src/atomic/*.c
  ${SDL2_SOURCE_DIR}/src/audio/*.c
  ${SDL2_SOURCE_DIR}/src/cpuinfo/*.c
  ${SDL2_SOURCE_DIR}/src/dynapi/*.c
  ${SDL2_SOURCE_DIR}/src/events/*.c
  ${SDL2_SOURCE_DIR}/src/file/*.c
  ${SDL2_SOURCE_DIR}/src/libm/*.c
  ${SDL2_SOURCE_DIR}/src/render/*.c
  ${SDL2_SOURCE_DIR}/src/render/*/*.c
  ${SDL2_SOURCE_DIR}/src/stdlib/*.c
  ${SDL2_SOURCE_DIR}/src/thread/*.c
  ${SDL2_SOURCE_DIR}/src/timer/*.c
  ${SDL2_SOURCE_DIR}/src/video/*.c
  ${SDL2_SOURCE_DIR}/src/video/yuv2rgb/*.c)


if(ASSERTIONS STREQUAL "auto")
  # Do nada - use optimization settings to determine the assertion level
elseif(ASSERTIONS STREQUAL "disabled")
  set(SDL_DEFAULT_ASSERT_LEVEL 0)
elseif(ASSERTIONS STREQUAL "release")
  set(SDL_DEFAULT_ASSERT_LEVEL 1)
elseif(ASSERTIONS STREQUAL "enabled")
  set(SDL_DEFAULT_ASSERT_LEVEL 2)
elseif(ASSERTIONS STREQUAL "paranoid")
  set(SDL_DEFAULT_ASSERT_LEVEL 3)
else()
  message_error("unknown assertion level")
endif()
set(HAVE_ASSERTIONS ${ASSERTIONS})

# Compiler option evaluation
if(USE_GCC OR USE_CLANG)
  # Check for -Wall first, so later things can override pieces of it.
  check_c_compiler_flag(-Wall HAVE_GCC_WALL)
  if(HAVE_GCC_WALL)
    list(APPEND EXTRA_CFLAGS "-Wall")
    if(HAIKU)
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-multichar")
    endif()
  endif()

  check_c_compiler_flag(-Wdeclaration-after-statement HAVE_GCC_WDECLARATION_AFTER_STATEMENT)
  if(HAVE_GCC_WDECLARATION_AFTER_STATEMENT)
    check_c_compiler_flag(-Werror=declaration-after-statement HAVE_GCC_WERROR_DECLARATION_AFTER_STATEMENT)
    #if(HAVE_GCC_WERROR_DECLARATION_AFTER_STATEMENT)
    #  list(APPEND EXTRA_CFLAGS "-Werror=declaration-after-statement")
    #endif()
    list(APPEND EXTRA_CFLAGS "-Wdeclaration-after-statement")
  endif()

  if(DEPENDENCY_TRACKING)
    check_c_source_compiles("
        #if !defined(__GNUC__) || __GNUC__ < 3
        #error Dependency tracking requires GCC 3.0 or newer
        #endif
        int main(int argc, char **argv) { }" HAVE_DEPENDENCY_TRACKING)
  endif()

  if(GCC_ATOMICS)
    check_c_source_compiles("int main(int argc, char **argv) {
        int a;
        void *x, *y, *z;
        __sync_lock_test_and_set(&a, 4);
        __sync_lock_test_and_set(&x, y);
        __sync_fetch_and_add(&a, 1);
        __sync_bool_compare_and_swap(&a, 5, 10);
        __sync_bool_compare_and_swap(&x, y, z); }" HAVE_GCC_ATOMICS)
    if(NOT HAVE_GCC_ATOMICS)
      check_c_source_compiles("int main(int argc, char **argv) {
          int a;
          __sync_lock_test_and_set(&a, 1);
          __sync_lock_release(&a); }" HAVE_GCC_SYNC_LOCK_TEST_AND_SET)
    endif()
  endif()

  set(CMAKE_REQUIRED_FLAGS "-mpreferred-stack-boundary=2")
  check_c_source_compiles("int x = 0; int main(int argc, char **argv) {}"
    HAVE_GCC_PREFERRED_STACK_BOUNDARY)
  set(CMAKE_REQUIRED_FLAGS ${ORIG_CMAKE_REQUIRED_FLAGS})

  set(CMAKE_REQUIRED_FLAGS "-fvisibility=hidden -Werror")
  check_c_source_compiles("
      #if !defined(__GNUC__) || __GNUC__ < 4
      #error SDL only uses visibility attributes in GCC 4 or newer
      #endif
      int main(int argc, char **argv) {}" HAVE_GCC_FVISIBILITY)
  if(HAVE_GCC_FVISIBILITY)
    list(APPEND EXTRA_CFLAGS "-fvisibility=hidden")
  endif()
  set(CMAKE_REQUIRED_FLAGS ${ORIG_CMAKE_REQUIRED_FLAGS})

  check_c_compiler_flag(-Wshadow HAVE_GCC_WSHADOW)
  if(HAVE_GCC_WSHADOW)
    list(APPEND EXTRA_CFLAGS "-Wshadow")
  endif()

  if(APPLE)
    list(APPEND EXTRA_LDFLAGS "-Wl,-undefined,error")
    list(APPEND EXTRA_LDFLAGS "-Wl,-compatibility_version,${DYLIB_COMPATIBILITY_VERSION}")
    list(APPEND EXTRA_LDFLAGS "-Wl,-current_version,${DYLIB_CURRENT_VERSION}")
  else()
    set(CMAKE_REQUIRED_FLAGS "-Wl,--no-undefined")
    check_c_compiler_flag("" HAVE_NO_UNDEFINED)
    set(CMAKE_REQUIRED_FLAGS ${ORIG_CMAKE_REQUIRED_FLAGS})
    if(HAVE_NO_UNDEFINED)
      list(APPEND EXTRA_LDFLAGS "-Wl,--no-undefined")
    endif()
  endif()
endif()

if(ASSEMBLY)
  if(USE_GCC OR USE_CLANG)
    set(SDL_ASSEMBLY_ROUTINES 1)
    # TODO: Those all seem to be quite GCC specific - needs to be
    # reworked for better compiler support
    set(HAVE_ASSEMBLY TRUE)
    if(MMX)
      set(CMAKE_REQUIRED_FLAGS "-mmmx")
      check_c_source_compiles("
          #ifdef __MINGW32__
          #include <_mingw.h>
          #ifdef __MINGW64_VERSION_MAJOR
          #include <intrin.h>
          #else
          #include <mmintrin.h>
          #endif
          #else
          #include <mmintrin.h>
          #endif
          #ifndef __MMX__
          #error Assembler CPP flag not enabled
          #endif
          int main(int argc, char **argv) { }" HAVE_MMX)
      if(HAVE_MMX)
        list(APPEND EXTRA_CFLAGS "-mmmx")
      endif()
      set(CMAKE_REQUIRED_FLAGS ${ORIG_CMAKE_REQUIRED_FLAGS})
    endif()

    if(3DNOW)
      set(CMAKE_REQUIRED_FLAGS "-m3dnow")
      check_c_source_compiles("
          #include <mm3dnow.h>
          #ifndef __3dNOW__
          #error Assembler CPP flag not enabled
          #endif
          int main(int argc, char **argv) {
            void *p = 0;
            _m_prefetch(p);
          }" HAVE_3DNOW)
      if(HAVE_3DNOW)
        list(APPEND EXTRA_CFLAGS "-m3dnow")
      endif()
      set(CMAKE_REQUIRED_FLAGS ${ORIG_CMAKE_REQUIRED_FLAGS})
    endif()

    if(SSE)
      set(CMAKE_REQUIRED_FLAGS "-msse")
      check_c_source_compiles("
          #ifdef __MINGW32__
          #include <_mingw.h>
          #ifdef __MINGW64_VERSION_MAJOR
          #include <intrin.h>
          #else
          #include <xmmintrin.h>
          #endif
          #else
          #include <xmmintrin.h>
          #endif
          #ifndef __SSE__
          #error Assembler CPP flag not enabled
          #endif
          int main(int argc, char **argv) { }" HAVE_SSE)
      if(HAVE_SSE)
        list(APPEND EXTRA_CFLAGS "-msse")
      endif()
      set(CMAKE_REQUIRED_FLAGS ${ORIG_CMAKE_REQUIRED_FLAGS})
    endif()

    if(SSE2)
      set(CMAKE_REQUIRED_FLAGS "-msse2")
      check_c_source_compiles("
          #ifdef __MINGW32__
          #include <_mingw.h>
          #ifdef __MINGW64_VERSION_MAJOR
          #include <intrin.h>
          #else
          #include <emmintrin.h>
          #endif
          #else
          #include <emmintrin.h>
          #endif
          #ifndef __SSE2__
          #error Assembler CPP flag not enabled
          #endif
          int main(int argc, char **argv) { }" HAVE_SSE2)
      if(HAVE_SSE2)
        list(APPEND EXTRA_CFLAGS "-msse2")
      endif()
      set(CMAKE_REQUIRED_FLAGS ${ORIG_CMAKE_REQUIRED_FLAGS})
    endif()

    if(SSE3)
      set(CMAKE_REQUIRED_FLAGS "-msse3")
      check_c_source_compiles("
          #ifdef __MINGW32__
          #include <_mingw.h>
          #ifdef __MINGW64_VERSION_MAJOR
          #include <intrin.h>
          #else
          #include <pmmintrin.h>
          #endif
          #else
          #include <pmmintrin.h>
          #endif
          #ifndef __SSE3__
          #error Assembler CPP flag not enabled
          #endif
          int main(int argc, char **argv) { }" HAVE_SSE3)
      if(HAVE_SSE3)
        list(APPEND EXTRA_CFLAGS "-msse3")
      endif()
      set(CMAKE_REQUIRED_FLAGS ${ORIG_CMAKE_REQUIRED_FLAGS})
    endif()

    if(NOT SSEMATH)
      if(SSE OR SSE2 OR SSE3)
        if(USE_GCC)
          check_c_compiler_flag(-mfpmath=387 HAVE_FP_387)
          if(HAVE_FP_387)
            list(APPEND EXTRA_CFLAGS "-mfpmath=387")
          endif()
        endif()
        set(HAVE_SSEMATH TRUE)
      endif()
    endif()

    check_include_file("immintrin.h" HAVE_IMMINTRIN_H)

    if(ALTIVEC)
      set(CMAKE_REQUIRED_FLAGS "-maltivec")
      check_c_source_compiles("
          #include <altivec.h>
          vector unsigned int vzero() {
              return vec_splat_u32(0);
          }
          int main(int argc, char **argv) { }" HAVE_ALTIVEC_H_HDR)
      check_c_source_compiles("
          vector unsigned int vzero() {
              return vec_splat_u32(0);
          }
          int main(int argc, char **argv) { }" HAVE_ALTIVEC)
      set(CMAKE_REQUIRED_FLAGS ${ORIG_CMAKE_REQUIRED_FLAGS})
      if(HAVE_ALTIVEC OR HAVE_ALTIVEC_H_HDR)
        set(HAVE_ALTIVEC TRUE) # if only HAVE_ALTIVEC_H_HDR is set
        list(APPEND EXTRA_CFLAGS "-maltivec")
        set(SDL_ALTIVEC_BLITTERS 1)
        if(HAVE_ALTIVEC_H_HDR)
          set(HAVE_ALTIVEC_H 1)
        endif()
      endif()
    endif()
  elseif(MSVC_VERSION GREATER 1500)
    # TODO: SDL_cpuinfo.h needs to support the user's configuration wish
    # for MSVC - right now it is always activated
    if(NOT ARCH_64)
      set(HAVE_MMX TRUE)
      set(HAVE_3DNOW TRUE)
    endif()
    set(HAVE_SSE TRUE)
    set(HAVE_SSE2 TRUE)
    set(HAVE_SSE3 TRUE)
    set(SDL_ASSEMBLY_ROUTINES 1)
  endif()
# TODO:
#else()
#  if(USE_GCC OR USE_CLANG)
#    list(APPEND EXTRA_CFLAGS "-mno-sse" "-mno-sse2" "-mno-sse3" "-mno-mmx")
#  endif()
endif()

# TODO: Can't deactivate on FreeBSD? w/o LIBC, SDL_stdinc.h can't define
# anything.
if(LIBC)
  if(WINDOWS AND NOT MINGW)
    set(HAVE_LIBC TRUE)
    foreach(_HEADER stdio.h string.h wchar.h ctype.h math.h limits.h)
      string(TOUPPER "HAVE_${_HEADER}" _UPPER)
      string(REPLACE "." "_" _HAVE_H ${_UPPER})
      set(${_HAVE_H} 1)
    endforeach()
    set(HAVE_SIGNAL_H 1)
    foreach(_FN
            malloc calloc realloc free qsort abs memset memcpy memmove memcmp
            wcslen wcscmp
            strlen _strrev _strupr _strlwr strchr strrchr strstr itoa _ltoa
            _ultoa strtol strtoul strtoll strtod atoi atof strcmp strncmp
            _stricmp _strnicmp sscanf
            acos acosf asin asinf atan atanf atan2 atan2f ceil ceilf
            copysign copysignf cos cosf exp expf fabs fabsf floor floorf fmod fmodf
            log logf log10 log10f pow powf scalbn scalbnf sin sinf sqrt sqrtf tan tanf)
      string(TOUPPER ${_FN} _UPPER)
      set(HAVE_${_UPPER} 1)
    endforeach()
    if(NOT CYGWIN AND NOT MINGW)
      set(HAVE_ALLOCA 1)
    endif()
    set(HAVE_M_PI 1)
    add_definitions(-D_USE_MATH_DEFINES) # needed for M_PI
    set(STDC_HEADERS 1)
  else()
    set(HAVE_LIBC TRUE)
    check_include_file(sys/types.h HAVE_SYS_TYPES_H)
    foreach(_HEADER
            stdio.h stdlib.h stddef.h stdarg.h malloc.h memory.h string.h limits.h
            strings.h wchar.h inttypes.h stdint.h ctype.h math.h iconv.h signal.h libunwind.h)
      string(TOUPPER "HAVE_${_HEADER}" _UPPER)
      string(REPLACE "." "_" _HAVE_H ${_UPPER})
      check_include_file("${_HEADER}" ${_HAVE_H})
    endforeach()

    check_include_files("dlfcn.h;stdint.h;stddef.h;inttypes.h;stdlib.h;strings.h;string.h;float.h" STDC_HEADERS)
    check_type_size("size_t" SIZEOF_SIZE_T)
    check_symbol_exists(M_PI math.h HAVE_M_PI)
    # TODO: refine the mprotect check
    check_c_source_compiles("#include <sys/types.h>
                             #include <sys/mman.h>
                             int main() { }" HAVE_MPROTECT)
    foreach(_FN
            strtod malloc calloc realloc free getenv setenv putenv unsetenv
            qsort abs bcopy memset memcpy memmove memcmp strlen strlcpy strlcat
            _strrev _strupr _strlwr strchr strrchr strstr itoa _ltoa
            _uitoa _ultoa strtol strtoul _i64toa _ui64toa strtoll strtoull
            atoi atof strcmp strncmp _stricmp strcasecmp _strnicmp strncasecmp
            vsscanf vsnprintf fopen64 fseeko fseeko64 sigaction setjmp
            nanosleep sysconf sysctlbyname getauxval poll
            )
      string(TOUPPER ${_FN} _UPPER)
      set(_HAVEVAR "HAVE_${_UPPER}")
      check_function_exists("${_FN}" ${_HAVEVAR})
    endforeach()

    check_library_exists(m pow "" HAVE_LIBM)
    if(HAVE_LIBM)
      set(CMAKE_REQUIRED_LIBRARIES m)
      foreach(_FN
              atan atan2 ceil copysign cos cosf fabs floor log pow scalbn sin
              sinf sqrt sqrtf tan tanf acos asin)
        string(TOUPPER ${_FN} _UPPER)
        set(_HAVEVAR "HAVE_${_UPPER}")
        check_function_exists("${_FN}" ${_HAVEVAR})
      endforeach()
      set(CMAKE_REQUIRED_LIBRARIES)
      list(APPEND EXTRA_LIBS m)
    endif()

    check_library_exists(iconv iconv_open "" HAVE_LIBICONV)
    if(HAVE_LIBICONV)
      list(APPEND EXTRA_LIBS iconv)
      set(HAVE_ICONV 1)
    endif()

    if(NOT APPLE)
      check_include_file(alloca.h HAVE_ALLOCA_H)
      check_function_exists(alloca HAVE_ALLOCA)
    else()
      set(HAVE_ALLOCA_H 1)
      set(HAVE_ALLOCA 1)
    endif()

    check_struct_has_member("struct sigaction" "sa_sigaction" "signal.h" HAVE_SA_SIGACTION)
  endif()
else()
  if(WINDOWS)
    set(HAVE_STDARG_H 1)
    set(HAVE_STDDEF_H 1)
  endif()
endif()


# Enable/disable various subsystems of the SDL library
foreach(_SUB ${SDL_SUBSYSTEMS})
  string(TOUPPER ${_SUB} _OPT)
  if(NOT SDL_${_OPT})
    set(SDL_${_OPT}_DISABLED 1)
  endif()
endforeach()
if(SDL_JOYSTICK)
  file(GLOB JOYSTICK_SOURCES ${SDL2_SOURCE_DIR}/src/joystick/*.c)
  set(SOURCE_FILES ${SOURCE_FILES} ${JOYSTICK_SOURCES})
endif()
if(SDL_HAPTIC)
  if(NOT SDL_JOYSTICK)
    # Haptic requires some private functions from the joystick subsystem.
    message_error("SDL_HAPTIC requires SDL_JOYSTICK, which is not enabled")
  endif()
  file(GLOB HAPTIC_SOURCES ${SDL2_SOURCE_DIR}/src/haptic/*.c)
  set(SOURCE_FILES ${SOURCE_FILES} ${HAPTIC_SOURCES})
endif()
if(SDL_SENSOR)
  file(GLOB SENSOR_SOURCES ${SDL2_SOURCE_DIR}/src/sensor/*.c)
  set(SOURCE_FILES ${SOURCE_FILES} ${SENSOR_SOURCES})
endif()
if(SDL_POWER)
  file(GLOB POWER_SOURCES ${SDL2_SOURCE_DIR}/src/power/*.c)
  set(SOURCE_FILES ${SOURCE_FILES} ${POWER_SOURCES})
endif()
# TODO: in configure.in, the test for LOADSO and SDL_DLOPEN is a bit weird:
# if LOADSO is not wanted, SDL_LOADSO_DISABLED is set
# If however on Unix or APPLE dlopen() is detected via CheckDLOPEN(),
# SDL_LOADSO_DISABLED will not be set, regardless of the LOADSO settings

# General SDL subsystem options, valid for all platforms
if(SDL_AUDIO)
  # CheckDummyAudio/CheckDiskAudio - valid for all platforms
  if(DUMMYAUDIO)
    set(SDL_AUDIO_DRIVER_DUMMY 1)
    file(GLOB DUMMYAUDIO_SOURCES ${SDL2_SOURCE_DIR}/src/audio/dummy/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${DUMMYAUDIO_SOURCES})
    set(HAVE_DUMMYAUDIO TRUE)
  endif()
  if(DISKAUDIO)
    set(SDL_AUDIO_DRIVER_DISK 1)
    file(GLOB DISKAUDIO_SOURCES ${SDL2_SOURCE_DIR}/src/audio/disk/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${DISKAUDIO_SOURCES})
    set(HAVE_DISKAUDIO TRUE)
  endif()
endif()

if(SDL_DLOPEN)
  # Relevant for Unix/Darwin only
  if(UNIX OR APPLE)
    CheckDLOPEN()
  endif()
endif()

if(SDL_VIDEO)
  if(VIDEO_DUMMY)
    set(SDL_VIDEO_DRIVER_DUMMY 1)
    file(GLOB VIDEO_DUMMY_SOURCES ${SDL2_SOURCE_DIR}/src/video/dummy/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${VIDEO_DUMMY_SOURCES})
    set(HAVE_VIDEO_DUMMY TRUE)
    set(HAVE_SDL_VIDEO TRUE)
  endif()
endif()

if(ANDROID)
  file(GLOB ANDROID_CORE_SOURCES ${SDL2_SOURCE_DIR}/src/core/android/*.c)
  set(SOURCE_FILES ${SOURCE_FILES} ${ANDROID_CORE_SOURCES})

  # SDL_spinlock.c Needs to be compiled in ARM mode.
  # There seems to be no better way currently to set the ARM mode.
  # see: https://issuetracker.google.com/issues/62264618
  # Another option would be to set ARM mode to all compiled files
  check_c_compiler_flag(-marm HAVE_ARM_MODE)
  if(HAVE_ARM_MODE)
    set_source_files_properties(${SDL2_SOURCE_DIR}/src/atomic/SDL_spinlock.c PROPERTIES COMPILE_FLAGS -marm)
  endif()

  file(GLOB ANDROID_MAIN_SOURCES ${SDL2_SOURCE_DIR}/src/main/android/*.c)
  set(SDLMAIN_SOURCES ${SDLMAIN_SOURCES} ${ANDROID_MAIN_SOURCES})

  if(SDL_AUDIO)
    set(SDL_AUDIO_DRIVER_ANDROID 1)
    file(GLOB ANDROID_AUDIO_SOURCES ${SDL2_SOURCE_DIR}/src/audio/android/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${ANDROID_AUDIO_SOURCES})
    set(HAVE_SDL_AUDIO TRUE)
  endif()
  if(SDL_FILESYSTEM)
    set(SDL_FILESYSTEM_ANDROID 1)
    file(GLOB ANDROID_FILESYSTEM_SOURCES ${SDL2_SOURCE_DIR}/src/filesystem/android/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${ANDROID_FILESYSTEM_SOURCES})
    set(HAVE_SDL_FILESYSTEM TRUE)
  endif()
  if(SDL_HAPTIC)
    set(SDL_HAPTIC_ANDROID 1)
    file(GLOB ANDROID_HAPTIC_SOURCES ${SDL2_SOURCE_DIR}/src/haptic/android/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${ANDROID_HAPTIC_SOURCES})
    set(HAVE_SDL_HAPTIC TRUE)
  endif()
  if(SDL_JOYSTICK)
    set(SDL_JOYSTICK_ANDROID 1)
    file(GLOB ANDROID_JOYSTICK_SOURCES ${SDL2_SOURCE_DIR}/src/joystick/android/*.c ${SDL2_SOURCE_DIR}/src/joystick/steam/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${ANDROID_JOYSTICK_SOURCES})
    set(HAVE_SDL_JOYSTICK TRUE)
  endif()
  if(SDL_LOADSO)
    set(SDL_LOADSO_DLOPEN 1)
    file(GLOB LOADSO_SOURCES ${SDL2_SOURCE_DIR}/src/loadso/dlopen/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${LOADSO_SOURCES})
    set(HAVE_SDL_LOADSO TRUE)
  endif()
  if(SDL_POWER)
    set(SDL_POWER_ANDROID 1)
    file(GLOB ANDROID_POWER_SOURCES ${SDL2_SOURCE_DIR}/src/power/android/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${ANDROID_POWER_SOURCES})
    set(HAVE_SDL_POWER TRUE)
  endif()
  if(SDL_TIMERS)
    set(SDL_TIMER_UNIX 1)
    file(GLOB TIMER_SOURCES ${SDL2_SOURCE_DIR}/src/timer/unix/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${TIMER_SOURCES})
    set(HAVE_SDL_TIMERS TRUE)
  endif()
  if(SDL_SENSOR)
    set(SDL_SENSOR_ANDROID 1)
    set(HAVE_SDL_SENSORS TRUE)
    file(GLOB ANDROID_SENSOR_SOURCES ${SDL2_SOURCE_DIR}/src/sensor/android/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${ANDROID_SENSOR_SOURCES})
  endif()
  if(SDL_VIDEO)
    set(SDL_VIDEO_DRIVER_ANDROID 1)
    file(GLOB ANDROID_VIDEO_SOURCES ${SDL2_SOURCE_DIR}/src/video/android/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${ANDROID_VIDEO_SOURCES})
    set(HAVE_SDL_VIDEO TRUE)

    # Core stuff
    find_library(ANDROID_DL_LIBRARY dl)
    find_library(ANDROID_LOG_LIBRARY log)
    find_library(ANDROID_LIBRARY_LIBRARY android)
    list(APPEND EXTRA_LIBS ${ANDROID_DL_LIBRARY} ${ANDROID_LOG_LIBRARY} ${ANDROID_LIBRARY_LIBRARY})
    add_definitions(-DGL_GLEXT_PROTOTYPES)

    #enable gles
    if(VIDEO_OPENGLES)
      set(SDL_VIDEO_OPENGL_EGL 1)
      set(HAVE_VIDEO_OPENGLES TRUE)
      set(SDL_VIDEO_OPENGL_ES2 1)
      set(SDL_VIDEO_RENDER_OGL_ES2 1)

      find_library(OpenGLES1_LIBRARY GLESv1_CM)
      find_library(OpenGLES2_LIBRARY GLESv2)
      list(APPEND EXTRA_LIBS ${OpenGLES1_LIBRARY} ${OpenGLES2_LIBRARY})
    endif()

    CHECK_C_SOURCE_COMPILES("
    #if defined(__ARM_ARCH) && __ARM_ARCH < 7
    #error Vulkan doesn't work on this configuration
    #endif
    int main()
    {
        return 0;
    }
    " VULKAN_PASSED_ANDROID_CHECKS)
    if(NOT VULKAN_PASSED_ANDROID_CHECKS)
      set(VIDEO_VULKAN OFF)
      message(STATUS "Vulkan doesn't work on this configuration")
    endif()
  endif()

  CheckPTHREAD()

endif()

# Platform-specific options and settings
if(EMSCRIPTEN)
  # Hide noisy warnings that intend to aid mostly during initial stages of porting a new
  # project. Uncomment at will for verbose cross-compiling -I/../ path info.
  add_definitions(-Wno-warn-absolute-paths)
  if(SDL_AUDIO)
    set(SDL_AUDIO_DRIVER_EMSCRIPTEN 1)
    file(GLOB EM_AUDIO_SOURCES ${SDL2_SOURCE_DIR}/src/audio/emscripten/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${EM_AUDIO_SOURCES})
    set(HAVE_SDL_AUDIO TRUE)
  endif()
  if(SDL_FILESYSTEM)
    set(SDL_FILESYSTEM_EMSCRIPTEN 1)
    file(GLOB EM_FILESYSTEM_SOURCES ${SDL2_SOURCE_DIR}/src/filesystem/emscripten/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${EM_FILESYSTEM_SOURCES})
    set(HAVE_SDL_FILESYSTEM TRUE)
  endif()
  if(SDL_JOYSTICK)
    set(SDL_JOYSTICK_EMSCRIPTEN 1)
    file(GLOB EM_JOYSTICK_SOURCES ${SDL2_SOURCE_DIR}/src/joystick/emscripten/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${EM_JOYSTICK_SOURCES})
    set(HAVE_SDL_JOYSTICK TRUE)
  endif()
  if(SDL_POWER)
    set(SDL_POWER_EMSCRIPTEN 1)
    file(GLOB EM_POWER_SOURCES ${SDL2_SOURCE_DIR}/src/power/emscripten/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${EM_POWER_SOURCES})
    set(HAVE_SDL_POWER TRUE)
  endif()
  if(SDL_TIMERS)
    set(SDL_TIMER_UNIX 1)
    file(GLOB TIMER_SOURCES ${SDL2_SOURCE_DIR}/src/timer/unix/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${TIMER_SOURCES})
    set(HAVE_SDL_TIMERS TRUE)

    if(CLOCK_GETTIME)
      set(HAVE_CLOCK_GETTIME 1)
    endif()
  endif()
  if(SDL_VIDEO)
    set(SDL_VIDEO_DRIVER_EMSCRIPTEN 1)
    file(GLOB EM_VIDEO_SOURCES ${SDL2_SOURCE_DIR}/src/video/emscripten/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${EM_VIDEO_SOURCES})
    set(HAVE_SDL_VIDEO TRUE)

    #enable gles
    if(VIDEO_OPENGLES)
      set(SDL_VIDEO_OPENGL_EGL 1)
      set(HAVE_VIDEO_OPENGLES TRUE)
      set(SDL_VIDEO_OPENGL_ES2 1)
      set(SDL_VIDEO_RENDER_OGL_ES2 1)
    endif()
  endif()
elseif(UNIX AND NOT APPLE AND NOT ANDROID)
  if(SDL_AUDIO)
    if(SYSV5 OR SOLARIS OR HPUX)
        set(SDL_AUDIO_DRIVER_SUNAUDIO 1)
        file(GLOB SUN_AUDIO_SOURCES ${SDL2_SOURCE_DIR}/src/audio/sun/*.c)
        set(SOURCE_FILES ${SOURCE_FILES} ${SUN_AUDIO_SOURCES})
        set(HAVE_SDL_AUDIO TRUE)
    elseif(NETBSD)
        set(SDL_AUDIO_DRIVER_NETBSD 1)
        file(GLOB NETBSD_AUDIO_SOURCES ${SDL2_SOURCE_DIR}/src/audio/netbsd/*.c)
        set(SOURCE_FILES ${SOURCE_FILES} ${NETBSD_AUDIO_SOURCES})
        set(HAVE_SDL_AUDIO TRUE)
    elseif(AIX)
        set(SDL_AUDIO_DRIVER_PAUDIO 1)
        file(GLOB AIX_AUDIO_SOURCES ${SDL2_SOURCE_DIR}/src/audio/paudio/*.c)
        set(SOURCE_FILES ${SOURCE_FILES} ${AIX_AUDIO_SOURCES})
        set(HAVE_SDL_AUDIO TRUE)
    endif()
    CheckOSS()
    CheckALSA()
    CheckJACK()
    CheckPulseAudio()
    CheckESD()
    CheckARTS()
    CheckNAS()
    CheckSNDIO()
    CheckFusionSound()
    CheckLibSampleRate()
  endif()

  if(SDL_VIDEO)
    # Need to check for Raspberry PI first and add platform specific compiler flags, otherwise the test for GLES fails!
    CheckRPI()
    CheckX11()
    CheckDirectFB()
    CheckOpenGLX11()
    CheckOpenGLESX11()
    CheckWayland()
    CheckVivante()
    CheckKMSDRM()
  endif()

  if(UNIX)
    file(GLOB CORE_UNIX_SOURCES ${SDL2_SOURCE_DIR}/src/core/unix/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${CORE_UNIX_SOURCES})
  endif()

  if(LINUX)
    check_c_source_compiles("
        #include <linux/input.h>
        #ifndef EVIOCGNAME
        #error EVIOCGNAME() ioctl not available
        #endif
        int main(int argc, char** argv) {}" HAVE_INPUT_EVENTS)

    check_c_source_compiles("
        #include <linux/kd.h>
        #include <linux/keyboard.h>

        int main(int argc, char **argv)
        {
            struct kbentry kbe;
            kbe.kb_table = KG_CTRL;
            ioctl(0, KDGKBENT, &kbe);
        }" HAVE_INPUT_KD)

    file(GLOB CORE_LINUX_SOURCES ${SDL2_SOURCE_DIR}/src/core/linux/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${CORE_LINUX_SOURCES})

    if(HAVE_INPUT_EVENTS)
      set(SDL_INPUT_LINUXEV 1)
    endif()

    if(SDL_HAPTIC AND HAVE_INPUT_EVENTS)
      set(SDL_HAPTIC_LINUX 1)
      file(GLOB HAPTIC_SOURCES ${SDL2_SOURCE_DIR}/src/haptic/linux/*.c)
      set(SOURCE_FILES ${SOURCE_FILES} ${HAPTIC_SOURCES})
      set(HAVE_SDL_HAPTIC TRUE)
    endif()

    if(HAVE_INPUT_KD)
      set(SDL_INPUT_LINUXKD 1)
    endif()

    check_include_file("libudev.h" HAVE_LIBUDEV_H)

    if(PKG_CONFIG_FOUND)
      pkg_search_module(DBUS dbus-1 dbus)
      if(DBUS_FOUND)
        set(HAVE_DBUS_DBUS_H TRUE)
        include_directories(${DBUS_INCLUDE_DIRS})
        list(APPEND EXTRA_LIBS ${DBUS_LIBRARIES})
      endif()

      pkg_search_module(IBUS ibus-1.0 ibus)
      if(IBUS_FOUND)
        set(HAVE_IBUS_IBUS_H TRUE)
        include_directories(${IBUS_INCLUDE_DIRS})
        list(APPEND EXTRA_LIBS ${IBUS_LIBRARIES})
      endif()
      if(HAVE_LIBUNWIND_H)
        # We've already found the header, so REQUIRE the lib to be present
        pkg_search_module(UNWIND REQUIRED libunwind)
        pkg_search_module(UNWIND_GENERIC REQUIRED libunwind-generic)
        list(APPEND EXTRA_LIBS ${UNWIND_LIBRARIES} ${UNWIND_GENERIC_LIBRARIES})
      endif()
    endif()

    check_include_file("fcitx/frontend.h" HAVE_FCITX_FRONTEND_H)
  endif()

  if(INPUT_TSLIB)
    check_c_source_compiles("
        #include \"tslib.h\"
        int main(int argc, char** argv) { }" HAVE_INPUT_TSLIB)
    if(HAVE_INPUT_TSLIB)
      set(SDL_INPUT_TSLIB 1)
      list(APPEND EXTRA_LIBS ts)
    endif()
  endif()

  if(SDL_JOYSTICK)
    CheckUSBHID()   # seems to be BSD specific - limit the test to BSD only?
    if(LINUX AND NOT ANDROID)
      set(SDL_JOYSTICK_LINUX 1)
      file(GLOB JOYSTICK_SOURCES ${SDL2_SOURCE_DIR}/src/joystick/linux/*.c ${SDL2_SOURCE_DIR}/src/joystick/steam/*.c)
      set(SOURCE_FILES ${SOURCE_FILES} ${JOYSTICK_SOURCES})
      set(HAVE_SDL_JOYSTICK TRUE)
    endif()
  endif()

  CheckPTHREAD()

  if(CLOCK_GETTIME)
    check_library_exists(rt clock_gettime "" FOUND_CLOCK_GETTIME)
    if(FOUND_CLOCK_GETTIME)
      list(APPEND EXTRA_LIBS rt)
      set(HAVE_CLOCK_GETTIME 1)
    else()
      check_library_exists(c clock_gettime "" FOUND_CLOCK_GETTIME)
      if(FOUND_CLOCK_GETTIME)
        set(HAVE_CLOCK_GETTIME 1)
      endif()
    endif()
  endif()

  check_include_file(linux/version.h HAVE_LINUX_VERSION_H)
  if(HAVE_LINUX_VERSION_H)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DHAVE_LINUX_VERSION_H")
  endif()

  if(SDL_POWER)
    if(LINUX)
      set(SDL_POWER_LINUX 1)
      file(GLOB POWER_SOURCES ${SDL2_SOURCE_DIR}/src/power/linux/*.c)
      set(SOURCE_FILES ${SOURCE_FILES} ${POWER_SOURCES})
      set(HAVE_SDL_POWER TRUE)
    endif()
  endif()

  if(SDL_FILESYSTEM)
    set(SDL_FILESYSTEM_UNIX 1)
    file(GLOB FILESYSTEM_SOURCES ${SDL2_SOURCE_DIR}/src/filesystem/unix/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${FILESYSTEM_SOURCES})
    set(HAVE_SDL_FILESYSTEM TRUE)
  endif()

  if(SDL_TIMERS)
    set(SDL_TIMER_UNIX 1)
    file(GLOB TIMER_SOURCES ${SDL2_SOURCE_DIR}/src/timer/unix/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${TIMER_SOURCES})
    set(HAVE_SDL_TIMERS TRUE)
  endif()

  if(RPATH)
    set(SDL_RLD_FLAGS "")
    if(BSDI OR FREEBSD OR LINUX OR NETBSD)
      set(CMAKE_REQUIRED_FLAGS "-Wl,--enable-new-dtags")
      check_c_compiler_flag("" HAVE_ENABLE_NEW_DTAGS)
      set(CMAKE_REQUIRED_FLAGS ${ORIG_CMAKE_REQUIRED_FLAGS})
      if(HAVE_ENABLE_NEW_DTAGS)
        set(SDL_RLD_FLAGS "-Wl,-rpath,\${libdir} -Wl,--enable-new-dtags")
      else()
        set(SDL_RLD_FLAGS "-Wl,-rpath,\${libdir}")
      endif()
    elseif(SOLARIS)
      set(SDL_RLD_FLAGS "-R\${libdir}")
    endif()
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    set(HAVE_RPATH TRUE)
  endif()

elseif(WINDOWS)
  find_program(WINDRES windres)

  check_c_source_compiles("
    #include <windows.h>
    int main(int argc, char **argv) { }" HAVE_WIN32_CC)

  file(GLOB CORE_SOURCES ${SDL2_SOURCE_DIR}/src/core/windows/*.c)
  set(SOURCE_FILES ${SOURCE_FILES} ${CORE_SOURCES})

  if(MSVC)
    # Prevent codegen that would use the VC runtime libraries.
    set_property(DIRECTORY . APPEND PROPERTY COMPILE_OPTIONS "/GS-")
    if(NOT ARCH_64)
      set_property(DIRECTORY . APPEND PROPERTY COMPILE_OPTIONS "/arch:SSE")
    endif()
  endif()

  # Check for DirectX
  if(DIRECTX)
    if(DEFINED MSVC_VERSION AND NOT ${MSVC_VERSION} LESS 1700)
        set(USE_WINSDK_DIRECTX TRUE)
    endif()
    if(NOT CMAKE_COMPILER_IS_MINGW AND NOT USE_WINSDK_DIRECTX)
      if("$ENV{DXSDK_DIR}" STREQUAL "")
        message_error("DIRECTX requires the \$DXSDK_DIR environment variable to be set")
      endif()
      set(CMAKE_REQUIRED_FLAGS "/I\"$ENV{DXSDK_DIR}\\Include\"")
    endif()

    if(HAVE_WIN32_CC)
      # xinput.h may need windows.h, but doesn't include it itself.
      check_c_source_compiles("
        #include <windows.h>
        #include <xinput.h>
        int main(int argc, char **argv) { }" HAVE_XINPUT_H)
      check_c_source_compiles("
        #include <windows.h>
        #include <xinput.h>
        XINPUT_GAMEPAD_EX x1;
        int main(int argc, char **argv) { }" HAVE_XINPUT_GAMEPAD_EX)
      check_c_source_compiles("
        #include <windows.h>
        #include <xinput.h>
        XINPUT_STATE_EX s1;
        int main(int argc, char **argv) { }" HAVE_XINPUT_STATE_EX)
    else()
      check_include_file(xinput.h HAVE_XINPUT_H)
    endif()

    check_include_file(d3d9.h HAVE_D3D_H)
    check_include_file(d3d11_1.h HAVE_D3D11_H)
    check_include_file(ddraw.h HAVE_DDRAW_H)
    check_include_file(dsound.h HAVE_DSOUND_H)
    check_include_file(dinput.h HAVE_DINPUT_H)
    check_include_file(dxgi.h HAVE_DXGI_H)
    if(HAVE_D3D_H OR HAVE_D3D11_H OR HAVE_DDRAW_H OR HAVE_DSOUND_H OR HAVE_DINPUT_H)
      set(HAVE_DIRECTX TRUE)
      if(NOT CMAKE_COMPILER_IS_MINGW AND NOT USE_WINSDK_DIRECTX)
      # TODO: change $ENV{DXSDL_DIR} to get the path from the include checks
        link_directories($ENV{DXSDK_DIR}\\lib\\${PROCESSOR_ARCH})
        include_directories($ENV{DXSDK_DIR}\\Include)
      endif()
    endif()
    set(CMAKE_REQUIRED_FLAGS ${ORIG_CMAKE_REQUIRED_FLAGS})
  endif()

  # headers needed elsewhere ...
  check_include_file(mmdeviceapi.h HAVE_MMDEVICEAPI_H)
  check_include_file(audioclient.h HAVE_AUDIOCLIENT_H)
  check_include_file(endpointvolume.h HAVE_ENDPOINTVOLUME_H)

  if(SDL_AUDIO)
    set(SDL_AUDIO_DRIVER_WINMM 1)
    file(GLOB WINMM_AUDIO_SOURCES ${SDL2_SOURCE_DIR}/src/audio/winmm/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${WINMM_AUDIO_SOURCES})
    set(HAVE_SDL_AUDIO TRUE)

    if(HAVE_DSOUND_H)
      set(SDL_AUDIO_DRIVER_DSOUND 1)
      file(GLOB DSOUND_AUDIO_SOURCES ${SDL2_SOURCE_DIR}/src/audio/directsound/*.c)
      set(SOURCE_FILES ${SOURCE_FILES} ${DSOUND_AUDIO_SOURCES})
    endif()

    if(WASAPI AND HAVE_AUDIOCLIENT_H AND HAVE_MMDEVICEAPI_H)
      set(SDL_AUDIO_DRIVER_WASAPI 1)
      file(GLOB WASAPI_AUDIO_SOURCES ${SDL2_SOURCE_DIR}/src/audio/wasapi/*.c)
      set(SOURCE_FILES ${SOURCE_FILES} ${WASAPI_AUDIO_SOURCES})
    endif()
  endif()

  if(SDL_VIDEO)
    # requires SDL_LOADSO on Windows (IME, DX, etc.)
    if(NOT SDL_LOADSO)
      message_error("SDL_VIDEO requires SDL_LOADSO, which is not enabled")
    endif()
    set(SDL_VIDEO_DRIVER_WINDOWS 1)
    file(GLOB WIN_VIDEO_SOURCES ${SDL2_SOURCE_DIR}/src/video/windows/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${WIN_VIDEO_SOURCES})

    if(RENDER_D3D AND HAVE_D3D_H)
      set(SDL_VIDEO_RENDER_D3D 1)
      set(HAVE_RENDER_D3D TRUE)
    endif()
    if(RENDER_D3D AND HAVE_D3D11_H)
      set(SDL_VIDEO_RENDER_D3D11 1)
      set(HAVE_RENDER_D3D TRUE)
    endif()
    set(HAVE_SDL_VIDEO TRUE)
  endif()

  if(SDL_THREADS)
    set(SDL_THREAD_WINDOWS 1)
    set(SOURCE_FILES ${SOURCE_FILES}
      ${SDL2_SOURCE_DIR}/src/thread/windows/SDL_sysmutex.c
      ${SDL2_SOURCE_DIR}/src/thread/windows/SDL_syssem.c
      ${SDL2_SOURCE_DIR}/src/thread/windows/SDL_systhread.c
      ${SDL2_SOURCE_DIR}/src/thread/windows/SDL_systls.c
      ${SDL2_SOURCE_DIR}/src/thread/generic/SDL_syscond.c)
    set(HAVE_SDL_THREADS TRUE)
  endif()

  if(SDL_POWER)
    set(SDL_POWER_WINDOWS 1)
    set(SOURCE_FILES ${SOURCE_FILES} ${SDL2_SOURCE_DIR}/src/power/windows/SDL_syspower.c)
    set(HAVE_SDL_POWER TRUE)
  endif()

  if(SDL_FILESYSTEM)
    set(SDL_FILESYSTEM_WINDOWS 1)
    file(GLOB FILESYSTEM_SOURCES ${SDL2_SOURCE_DIR}/src/filesystem/windows/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${FILESYSTEM_SOURCES})
    set(HAVE_SDL_FILESYSTEM TRUE)
  endif()

  # Libraries for Win32 native and MinGW
  list(APPEND EXTRA_LIBS user32 gdi32 winmm imm32 ole32 oleaut32 version uuid advapi32 shell32)

  # TODO: in configure.in the check for timers is set on
  # cygwin | mingw32* - does this include mingw32CE?
  if(SDL_TIMERS)
    set(SDL_TIMER_WINDOWS 1)
    file(GLOB TIMER_SOURCES ${SDL2_SOURCE_DIR}/src/timer/windows/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${TIMER_SOURCES})
    set(HAVE_SDL_TIMERS TRUE)
  endif()

  if(SDL_LOADSO)
    set(SDL_LOADSO_WINDOWS 1)
    file(GLOB LOADSO_SOURCES ${SDL2_SOURCE_DIR}/src/loadso/windows/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${LOADSO_SOURCES})
    set(HAVE_SDL_LOADSO TRUE)
  endif()

  file(GLOB CORE_SOURCES ${SDL2_SOURCE_DIR}/src/core/windows/*.c)
  set(SOURCE_FILES ${SOURCE_FILES} ${CORE_SOURCES})

  if(SDL_VIDEO)
    if(VIDEO_OPENGL)
      set(SDL_VIDEO_OPENGL 1)
      set(SDL_VIDEO_OPENGL_WGL 1)
      set(SDL_VIDEO_RENDER_OGL 1)
      set(HAVE_VIDEO_OPENGL TRUE)
    endif()

    if(VIDEO_OPENGLES)
      set(SDL_VIDEO_OPENGL_EGL 1)
      set(SDL_VIDEO_OPENGL_ES2 1)
      set(SDL_VIDEO_RENDER_OGL_ES2 1)
      set(HAVE_VIDEO_OPENGLES TRUE)
    endif()
  endif()

  if(SDL_JOYSTICK)
    file(GLOB JOYSTICK_SOURCES ${SDL2_SOURCE_DIR}/src/joystick/windows/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${JOYSTICK_SOURCES})
    if(HAVE_DINPUT_H)
      set(SDL_JOYSTICK_DINPUT 1)
      list(APPEND EXTRA_LIBS dinput8)
      if(CMAKE_COMPILER_IS_MINGW)
        list(APPEND EXTRA_LIBS dxerr8)
      elseif (NOT USE_WINSDK_DIRECTX)
        list(APPEND EXTRA_LIBS dxerr)
      endif()
    endif()
    if(HAVE_XINPUT_H)
      set(SDL_JOYSTICK_XINPUT 1)
    endif()
    if(NOT HAVE_DINPUT_H AND NOT HAVE_XINPUT_H)
      set(SDL_JOYSTICK_WINMM 1)
    endif()
    set(HAVE_SDL_JOYSTICK TRUE)

    if(SDL_HAPTIC)
      if(HAVE_DINPUT_H OR HAVE_XINPUT_H)
        file(GLOB HAPTIC_SOURCES ${SDL2_SOURCE_DIR}/src/haptic/windows/*.c)
        if(HAVE_DINPUT_H)
          set(SDL_HAPTIC_DINPUT 1)
        endif()
        if(HAVE_XINPUT_H)
          set(SDL_HAPTIC_XINPUT 1)
        endif()
      else()
        file(GLOB HAPTIC_SOURCES ${SDL2_SOURCE_DIR}/src/haptic/dummy/*.c)
        set(SDL_HAPTIC_DUMMY 1)
      endif()
      set(SOURCE_FILES ${SOURCE_FILES} ${HAPTIC_SOURCES})
      set(HAVE_SDL_HAPTIC TRUE)
    endif()
  endif()

  file(GLOB VERSION_SOURCES ${SDL2_SOURCE_DIR}/src/main/windows/*.rc)
  file(GLOB SDLMAIN_SOURCES ${SDL2_SOURCE_DIR}/src/main/windows/*.c)
  if(MINGW OR CYGWIN)
    list(APPEND EXTRA_LIBS mingw32)
    list(APPEND EXTRA_LDFLAGS "-mwindows")
    set(SDL_CFLAGS "${SDL_CFLAGS} -Dmain=SDL_main")
    list(APPEND SDL_LIBS "-lmingw32" "-lSDL2main" "-mwindows")
  endif()
elseif(APPLE)
  # TODO: rework this all for proper MacOS X, iOS and Darwin support

  # We always need these libs on macOS at the moment.
  # !!! FIXME: we need Carbon for some very old API calls in
  # !!! FIXME:  src/video/cocoa/SDL_cocoakeyboard.c, but we should figure out
  # !!! FIXME:  how to dump those.
  if(NOT IOS)
    set(SDL_FRAMEWORK_COCOA 1)
    set(SDL_FRAMEWORK_CARBON 1)
  endif()

  # Requires the darwin file implementation
  if(SDL_FILE)
    file(GLOB EXTRA_SOURCES ${SDL2_SOURCE_DIR}/src/file/cocoa/*.m)
    set(SOURCE_FILES ${EXTRA_SOURCES} ${SOURCE_FILES})
    # !!! FIXME: modern CMake doesn't need "LANGUAGE C" for Objective-C.
    set_source_files_properties(${EXTRA_SOURCES} PROPERTIES LANGUAGE C)
    set(HAVE_SDL_FILE TRUE)
    # !!! FIXME: why is COREVIDEO inside this if() block?
    set(SDL_FRAMEWORK_COREVIDEO 1)
  else()
    message_error("SDL_FILE must be enabled to build on MacOS X")
  endif()

  if(SDL_AUDIO)
    set(SDL_AUDIO_DRIVER_COREAUDIO 1)
    file(GLOB AUDIO_SOURCES ${SDL2_SOURCE_DIR}/src/audio/coreaudio/*.m)
    # !!! FIXME: modern CMake doesn't need "LANGUAGE C" for Objective-C.
    set_source_files_properties(${AUDIO_SOURCES} PROPERTIES LANGUAGE C)
    set(SOURCE_FILES ${SOURCE_FILES} ${AUDIO_SOURCES})
    set(HAVE_SDL_AUDIO TRUE)
    set(SDL_FRAMEWORK_COREAUDIO 1)
    set(SDL_FRAMEWORK_AUDIOTOOLBOX 1)
  endif()

  if(SDL_JOYSTICK)
    set(SDL_JOYSTICK_IOKIT 1)
    if (IOS)
      file(GLOB JOYSTICK_SOURCES ${SDL2_SOURCE_DIR}/src/joystick/iphoneos/*.m ${SDL2_SOURCE_DIR}/src/joystick/steam/*.c)
    else()
      file(GLOB JOYSTICK_SOURCES ${SDL2_SOURCE_DIR}/src/joystick/darwin/*.c)
    endif()
    set(SOURCE_FILES ${SOURCE_FILES} ${JOYSTICK_SOURCES})
    set(HAVE_SDL_JOYSTICK TRUE)
    set(SDL_FRAMEWORK_IOKIT 1)
    set(SDL_FRAMEWORK_FF 1)
  endif()

  if(SDL_HAPTIC)
    set(SDL_HAPTIC_IOKIT 1)
    if (IOS)
      file(GLOB HAPTIC_SOURCES ${SDL2_SOURCE_DIR}/src/haptic/dummy/*.c)
      set(SDL_HAPTIC_DUMMY 1)
    else()
      file(GLOB HAPTIC_SOURCES ${SDL2_SOURCE_DIR}/src/haptic/darwin/*.c)
    endif()
    set(SOURCE_FILES ${SOURCE_FILES} ${HAPTIC_SOURCES})
    set(HAVE_SDL_HAPTIC TRUE)
    set(SDL_FRAMEWORK_IOKIT 1)
    set(SDL_FRAMEWORK_FF 1)
    if(NOT SDL_JOYSTICK)
      message(FATAL_ERROR "SDL_HAPTIC requires SDL_JOYSTICK to be enabled")
    endif()
  endif()

  if(SDL_POWER)
    set(SDL_POWER_MACOSX 1)
    if (IOS)
      file(GLOB POWER_SOURCES ${SDL2_SOURCE_DIR}/src/power/uikit/*.m)
    else()
      file(GLOB POWER_SOURCES ${SDL2_SOURCE_DIR}/src/power/macosx/*.c)
    endif()
    set(SOURCE_FILES ${SOURCE_FILES} ${POWER_SOURCES})
    set(HAVE_SDL_POWER TRUE)
    set(SDL_FRAMEWORK_IOKIT 1)
  endif()

  if(SDL_TIMERS)
    set(SDL_TIMER_UNIX 1)
    file(GLOB TIMER_SOURCES ${SDL2_SOURCE_DIR}/src/timer/unix/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${TIMER_SOURCES})
    set(HAVE_SDL_TIMERS TRUE)
  endif(SDL_TIMERS)

  if(SDL_FILESYSTEM)
    set(SDL_FILESYSTEM_COCOA 1)
    file(GLOB FILESYSTEM_SOURCES ${SDL2_SOURCE_DIR}/src/filesystem/cocoa/*.m)
    # !!! FIXME: modern CMake doesn't need "LANGUAGE C" for Objective-C.
    set_source_files_properties(${FILESYSTEM_SOURCES} PROPERTIES LANGUAGE C)
    set(SOURCE_FILES ${SOURCE_FILES} ${FILESYSTEM_SOURCES})
    set(HAVE_SDL_FILESYSTEM TRUE)
  endif()

  # Actually load the frameworks at the end so we don't duplicate include.
  if(SDL_FRAMEWORK_COREVIDEO)
    find_library(COREVIDEO CoreVideo)
    list(APPEND EXTRA_LIBS ${COREVIDEO})
  endif()
  if(SDL_FRAMEWORK_COCOA)
    find_library(COCOA_LIBRARY Cocoa)
    list(APPEND EXTRA_LIBS ${COCOA_LIBRARY})
  endif()
  if(SDL_FRAMEWORK_IOKIT)
    find_library(IOKIT IOKit)
    list(APPEND EXTRA_LIBS ${IOKIT})
  endif()
  if(SDL_FRAMEWORK_FF)
    find_library(FORCEFEEDBACK ForceFeedback)
    list(APPEND EXTRA_LIBS ${FORCEFEEDBACK})
  endif()
  if(SDL_FRAMEWORK_CARBON)
    find_library(CARBON_LIBRARY Carbon)
    list(APPEND EXTRA_LIBS ${CARBON_LIBRARY})
  endif()
  if(SDL_FRAMEWORK_COREAUDIO)
    find_library(COREAUDIO CoreAudio)
    list(APPEND EXTRA_LIBS ${COREAUDIO})
  endif()
  if(SDL_FRAMEWORK_AUDIOTOOLBOX)
    find_library(AUDIOTOOLBOX AudioToolbox)
    list(APPEND EXTRA_LIBS ${AUDIOTOOLBOX})
  endif()

  # iOS hack needed - http://code.google.com/p/ios-cmake/ ?
  if(SDL_VIDEO)
    if (IOS)
      set(SDL_VIDEO_DRIVER_UIKIT 1)
      file(GLOB UIKITVIDEO_SOURCES ${SDL2_SOURCE_DIR}/src/video/uikit/*.m)
      set(SOURCE_FILES ${SOURCE_FILES} ${UIKITVIDEO_SOURCES})
    else()
      CheckCOCOA()
      if(VIDEO_OPENGL)
        set(SDL_VIDEO_OPENGL 1)
        set(SDL_VIDEO_OPENGL_CGL 1)
        set(SDL_VIDEO_RENDER_OGL 1)
        set(HAVE_VIDEO_OPENGL TRUE)
      endif()

      if(VIDEO_OPENGLES)
        set(SDL_VIDEO_OPENGL_EGL 1)
        set(SDL_VIDEO_OPENGL_ES2 1)
        set(SDL_VIDEO_RENDER_OGL_ES2 1)
        set(HAVE_VIDEO_OPENGLES TRUE)
      endif()
    endif()
  endif()

  CheckPTHREAD()
elseif(HAIKU)
  if(SDL_VIDEO)
    set(SDL_VIDEO_DRIVER_HAIKU 1)
    file(GLOB HAIKUVIDEO_SOURCES ${SDL2_SOURCE_DIR}/src/video/haiku/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${HAIKUVIDEO_SOURCES})
    set(HAVE_SDL_VIDEO TRUE)

    set(SDL_FILESYSTEM_HAIKU 1)
    file(GLOB FILESYSTEM_SOURCES ${SDL2_SOURCE_DIR}/src/filesystem/haiku/*.cc)
    set(SOURCE_FILES ${SOURCE_FILES} ${FILESYSTEM_SOURCES})
    set(HAVE_SDL_FILESYSTEM TRUE)

    if(SDL_TIMERS)
      set(SDL_TIMER_HAIKU 1)
      file(GLOB TIMER_SOURCES ${SDL2_SOURCE_DIR}/src/timer/haiku/*.c)
      set(SOURCE_FILES ${SOURCE_FILES} ${TIMER_SOURCES})
      set(HAVE_SDL_TIMERS TRUE)
    endif(SDL_TIMERS)

    if(VIDEO_OPENGL)
      # TODO: Use FIND_PACKAGE(OpenGL) instead
      set(SDL_VIDEO_OPENGL 1)
      set(SDL_VIDEO_OPENGL_BGL 1)
      set(SDL_VIDEO_RENDER_OGL 1)
      list(APPEND EXTRA_LIBS GL)
      set(HAVE_VIDEO_OPENGL TRUE)
    endif()
  endif()

  CheckPTHREAD()
endif()

if(VIDEO_VULKAN)
  set(SDL_VIDEO_VULKAN 1)
  set(HAVE_VIDEO_VULKAN TRUE)
endif()

# Dummies
# configure.in does it differently:
# if not have X
#   if enable_X {  SDL_X_DISABLED = 1 }
#   [add dummy sources]
# so it always adds a dummy, without checking, if it was actually requested.
# This leads to missing internal references on building, since the
# src/X/*.c does not get included.
if(NOT HAVE_SDL_JOYSTICK)
  set(SDL_JOYSTICK_DUMMY 1)
  if(SDL_JOYSTICK AND NOT APPLE) # results in unresolved symbols on OSX

    file(GLOB JOYSTICK_SOURCES ${SDL2_SOURCE_DIR}/src/joystick/dummy/*.c)
    set(SOURCE_FILES ${SOURCE_FILES} ${JOYSTICK_SOURCES})
  endif()
endif()
if(NOT HAVE_SDL_HAPTIC)
  set(SDL_HAPTIC_DUMMY 1)
  file(GLOB HAPTIC_SOURCES ${SDL2_SOURCE_DIR}/src/haptic/dummy/*.c)
  set(SOURCE_FILES ${SOURCE_FILES} ${HAPTIC_SOURCES})
endif()
if(NOT HAVE_SDL_SENSORS)
  set(SDL_SENSOR_DUMMY 1)
  file(GLOB SENSORS_SOURCES ${SDL2_SOURCE_DIR}/src/sensor/dummy/*.c)
  set(SOURCE_FILES ${SOURCE_FILES} ${SENSORS_SOURCES})
endif()
if(NOT HAVE_SDL_LOADSO)
  set(SDL_LOADSO_DISABLED 1)
  file(GLOB LOADSO_SOURCES ${SDL2_SOURCE_DIR}/src/loadso/dummy/*.c)
  set(SOURCE_FILES ${SOURCE_FILES} ${LOADSO_SOURCES})
endif()
if(NOT HAVE_SDL_FILESYSTEM)
  set(SDL_FILESYSTEM_DISABLED 1)
  file(GLOB FILESYSTEM_SOURCES ${SDL2_SOURCE_DIR}/src/filesystem/dummy/*.c)
  set(SOURCE_FILES ${SOURCE_FILES} ${FILESYSTEM_SOURCES})
endif()

# We always need to have threads and timers around
if(NOT HAVE_SDL_THREADS)
  set(SDL_THREADS_DISABLED 1)
  file(GLOB THREADS_SOURCES ${SDL2_SOURCE_DIR}/src/thread/generic/*.c)
  set(SOURCE_FILES ${SOURCE_FILES} ${THREADS_SOURCES})
endif()
if(NOT HAVE_SDL_TIMERS)
  set(SDL_TIMERS_DISABLED 1)
  file(GLOB TIMER_SOURCES ${SDL2_SOURCE_DIR}/src/timer/dummy/*.c)
  set(SOURCE_FILES ${SOURCE_FILES} ${TIMER_SOURCES})
endif()

if(NOT SDLMAIN_SOURCES)
  file(GLOB SDLMAIN_SOURCES ${SDL2_SOURCE_DIR}/src/main/dummy/*.c)
endif()

# Append the -MMD -MT flags
# if(DEPENDENCY_TRACKING)
#   if(COMPILER_IS_GNUCC)
#     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -MMD -MT \$@")
#   endif()
# endif()

configure_file("${SDL2_SOURCE_DIR}/include/SDL_config.h.cmake"
  "${SDL2_BINARY_DIR}/include/SDL_config.h")

# Prepare the flags and remove duplicates
if(EXTRA_LDFLAGS)
  list(REMOVE_DUPLICATES EXTRA_LDFLAGS)
endif()
if(EXTRA_LIBS)
  list(REMOVE_DUPLICATES EXTRA_LIBS)
endif()
if(EXTRA_CFLAGS)
  list(REMOVE_DUPLICATES EXTRA_CFLAGS)
endif()
listtostr(EXTRA_CFLAGS _EXTRA_CFLAGS)
set(EXTRA_CFLAGS ${_EXTRA_CFLAGS})

# Compat helpers for the configuration files
if(NOT WINDOWS OR CYGWIN)
  # TODO: we need a Windows script, too
  execute_process(COMMAND sh ${SDL2_SOURCE_DIR}/build-scripts/updaterev.sh)

  set(prefix ${CMAKE_INSTALL_PREFIX})
  set(exec_prefix "\${prefix}")
  set(libdir "\${exec_prefix}/lib${LIB_SUFFIX}")
  set(bindir "\${exec_prefix}/bin")
  set(includedir "\${prefix}/include")
  if(SDL_STATIC)
    set(ENABLE_STATIC_TRUE "")
    set(ENABLE_STATIC_FALSE "#")
  else()
    set(ENABLE_STATIC_TRUE "#")
    set(ENABLE_STATIC_FALSE "")
  endif()
  if(SDL_SHARED)
    set(ENABLE_SHARED_TRUE "")
    set(ENABLE_SHARED_FALSE "#")
  else()
    set(ENABLE_SHARED_TRUE "#")
    set(ENABLE_SHARED_FALSE "")
  endif()

  # Clean up the different lists
  listtostr(EXTRA_LIBS _EXTRA_LIBS "-l")
  set(SDL_STATIC_LIBS ${SDL_LIBS} ${EXTRA_LDFLAGS} ${_EXTRA_LIBS})
  list(REMOVE_DUPLICATES SDL_STATIC_LIBS)
  listtostr(SDL_STATIC_LIBS _SDL_STATIC_LIBS)
  set(SDL_STATIC_LIBS ${_SDL_STATIC_LIBS})
  listtostr(SDL_LIBS _SDL_LIBS)
  set(SDL_LIBS ${_SDL_LIBS})

  # MESSAGE(STATUS "SDL_LIBS: ${SDL_LIBS}")
  # MESSAGE(STATUS "SDL_STATIC_LIBS: ${SDL_STATIC_LIBS}")

  configure_file("${SDL2_SOURCE_DIR}/sdl2.pc.in"
    "${SDL2_BINARY_DIR}/sdl2.pc" @ONLY)
  configure_file("${SDL2_SOURCE_DIR}/sdl2-config.in"
    "${SDL2_BINARY_DIR}/sdl2-config")
  configure_file("${SDL2_SOURCE_DIR}/sdl2-config.in"
    "${SDL2_BINARY_DIR}/sdl2-config" @ONLY)
  configure_file("${SDL2_SOURCE_DIR}/SDL2.spec.in"
    "${SDL2_BINARY_DIR}/SDL2.spec" @ONLY)
endif()

##### Info output #####
message(STATUS "")
message(STATUS "SDL2 was configured with the following options:")
message(STATUS "")
message(STATUS "Platform: ${CMAKE_SYSTEM}")
message(STATUS "64-bit:   ${ARCH_64}")
message(STATUS "Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "")
message(STATUS "Subsystems:")
foreach(_SUB ${SDL_SUBSYSTEMS})
  string(TOUPPER ${_SUB} _OPT)
  message_bool_option(${_SUB} SDL_${_OPT})
endforeach()
message(STATUS "")
message(STATUS "Options:")
list(SORT ALLOPTIONS)
foreach(_OPT ${ALLOPTIONS})
  # Longest option is VIDEO_X11_XSCREENSAVER = 22 characters
  # Get the padding
  string(LENGTH ${_OPT} _OPTLEN)
  math(EXPR _PADLEN "23 - ${_OPTLEN}")
  string(RANDOM LENGTH ${_PADLEN} ALPHABET " " _PADDING)
  message_tested_option(${_OPT} ${_PADDING})
endforeach()
message(STATUS "")
message(STATUS " CFLAGS:        ${CMAKE_C_FLAGS}")
message(STATUS " EXTRA_CFLAGS:  ${EXTRA_CFLAGS}")
message(STATUS " EXTRA_LDFLAGS: ${EXTRA_LDFLAGS}")
message(STATUS " EXTRA_LIBS:    ${EXTRA_LIBS}")
message(STATUS "")
message(STATUS " Build Shared Library: ${SDL_SHARED}")
message(STATUS " Build Static Library: ${SDL_STATIC}")
if(SDL_STATIC)
    message(STATUS " Build Static Library with Position Independent Code: ${SDL_STATIC_PIC}")
endif()
message(STATUS "")
if(UNIX)
  message(STATUS "If something was not detected, although the libraries")
  message(STATUS "were installed, then make sure you have set the")
  message(STATUS "CFLAGS and LDFLAGS environment variables correctly.")
  message(STATUS "")
endif()

# Ensure that the extra cflags are used at compile time
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS}")

# Always build SDLmain
add_library(SDL2main STATIC ${SDLMAIN_SOURCES})
target_include_directories(SDL2main PUBLIC "$<BUILD_INTERFACE:${SDL2_SOURCE_DIR}/include>" $<INSTALL_INTERFACE:include/SDL2>)
set(_INSTALL_LIBS "SDL2main")
if (NOT ANDROID AND NOT ${SDL_CMAKE_DEBUG_POSTFIX} STREQUAL "")
  set_target_properties(SDL2main PROPERTIES DEBUG_POSTFIX ${SDL_CMAKE_DEBUG_POSTFIX})
endif()

set(SDL2_LIBRARIES_INTERNAL SDL2main)

if(SDL_SHARED)
  add_library(SDL2 SHARED ${SOURCE_FILES} ${VERSION_SOURCES})
  if(APPLE)
    set_target_properties(SDL2 PROPERTIES
      MACOSX_RPATH 1
      OUTPUT_NAME "SDL2-${LT_RELEASE}")
  elseif(UNIX AND NOT ANDROID)
    set_target_properties(SDL2 PROPERTIES
      VERSION ${LT_VERSION}
      SOVERSION ${LT_REVISION}
      OUTPUT_NAME "SDL2-${LT_RELEASE}")
  else()
    set_target_properties(SDL2 PROPERTIES
      VERSION ${SDL_VERSION}
      SOVERSION ${LT_REVISION}
      OUTPUT_NAME "SDL2")
  endif()
  if(MSVC AND NOT LIBC)
    # Don't try to link with the default set of libraries.
    set_target_properties(SDL2 PROPERTIES LINK_FLAGS_RELEASE "/NODEFAULTLIB")
    set_target_properties(SDL2 PROPERTIES LINK_FLAGS_DEBUG "/NODEFAULTLIB")
    set_target_properties(SDL2 PROPERTIES STATIC_LIBRARY_FLAGS "/NODEFAULTLIB")
  endif()
  set(_INSTALL_LIBS "SDL2" ${_INSTALL_LIBS})
  target_link_libraries(SDL2 ${EXTRA_LIBS} ${EXTRA_LDFLAGS})
  target_include_directories(SDL2 PUBLIC "$<BUILD_INTERFACE:${SDL2_SOURCE_DIR}/include>" $<INSTALL_INTERFACE:include/SDL2>)
  if (NOT ANDROID AND NOT ${SDL_CMAKE_DEBUG_POSTFIX} STREQUAL "")
    set_target_properties(SDL2 PROPERTIES DEBUG_POSTFIX ${SDL_CMAKE_DEBUG_POSTFIX})
  endif()

  set(SDL2_LIBRARIES_INTERNAL ${SDL2_LIBRARIES_INTERNAL} SDL2)
endif()

if(SDL_STATIC)
  set (BUILD_SHARED_LIBS FALSE)
  add_library(SDL2-static STATIC ${SOURCE_FILES})
  if (NOT SDL_SHARED OR NOT WIN32)
    set_target_properties(SDL2-static PROPERTIES OUTPUT_NAME "SDL2")
    # Note: Apparently, OUTPUT_NAME must really be unique; even when
    # CMAKE_IMPORT_LIBRARY_SUFFIX or the like are given. Otherwise
    # the static build may race with the import lib and one will get
    # clobbered, when the suffix is realized via subsequent rename.
  endif()
  set_target_properties(SDL2-static PROPERTIES POSITION_INDEPENDENT_CODE ${SDL_STATIC_PIC})
  if(MSVC AND NOT LIBC)
    set_target_properties(SDL2-static PROPERTIES LINK_FLAGS_RELEASE "/NODEFAULTLIB")
    set_target_properties(SDL2-static PROPERTIES LINK_FLAGS_DEBUG "/NODEFAULTLIB")
    set_target_properties(SDL2-static PROPERTIES STATIC_LIBRARY_FLAGS "/NODEFAULTLIB")
  endif()
  # TODO: Win32 platforms keep the same suffix .lib for import and static
  # libraries - do we need to consider this?
  set(_INSTALL_LIBS "SDL2-static" ${_INSTALL_LIBS})
  target_link_libraries(SDL2-static ${EXTRA_LIBS} ${EXTRA_LDFLAGS})
  target_include_directories(SDL2-static PUBLIC "$<BUILD_INTERFACE:${SDL2_SOURCE_DIR}/include>" $<INSTALL_INTERFACE:include/SDL2>)
  if (NOT ANDROID AND NOT ${SDL_CMAKE_DEBUG_POSTFIX} STREQUAL "")
    set_target_properties(SDL2-static PROPERTIES DEBUG_POSTFIX ${SDL_CMAKE_DEBUG_POSTFIX})
  endif()

  set(SDL2_LIBRARIES_INTERNAL ${SDL2_LIBRARIES_INTERNAL} SDL2-static)
endif()

## Finishing
set(SDL2_LIBRARIES ${SDL2_LIBRARIES_INTERNAL} PARENT_SCOPE)
set(SDL2_INCLUDE_DIRS ${SDL2_SOURCE_DIR}/src PARENT_SCOPE)

###
# Galogen - GL header generator
###
set(GALOGEN_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/galogen)
set(GALOGEN_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/galogen)
set(GALOGEN_SRCS "${GALOGEN_SRC_DIR}/galogen.cpp" "${GALOGEN_SRC_DIR}/third_party/tinyxml2.cpp")
add_executable(Galogen ${GALOGEN_SRCS})
#set_target_properties(Galogen PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${GALOGEN_BIN_DIR}) # This does not work for some reason

###
# STB
###
set(STB_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/stb PARENT_SCOPE)

###
# RapidJSON
###
set(RapidJSON_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include PARENT_SCOPE)

###
# ogg
###
set(OGG_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/ogg)
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

get_version_info(OGG_VERSION_INFO "LIB_CURRENT" "LIB_AGE" "LIB_REVISION")
set_target_properties(
    ogg PROPERTIES
    SOVERSION ${OGG_VERSION_INFO}
    PUBLIC_HEADER "${OGG_HEADERS}"
)


###
# Vorbis
###
set(VORBIS_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/vorbis)
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
set(OGG_ROOT ${OGG_SOURCE_DIR})
find_path(OGG_INCLUDE_DIRS NAMES ogg/ogg.h HINTS ${OGG_ROOT}/include PATH_SUFFIXES ogg)
find_library(OGG_LIBRARIES NAMES ogg HINTS ${OGG_ROOT}/lib ${OGG_ROOT}/lib64)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OGG DEFAULT_MSG OGG_INCLUDE_DIRS OGG_LIBRARIES)

add_subdirectory(${VORBIS_SOURCE_DIR}/lib)

###
# OpenAL
###
set(OpenAL_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/openal-soft)
set(OpenAL_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/openal-soft)

IF(COMMAND CMAKE_POLICY)
    CMAKE_POLICY(SET CMP0003 NEW)
    CMAKE_POLICY(SET CMP0005 NEW)
    IF(POLICY CMP0020)
        CMAKE_POLICY(SET CMP0020 NEW)
    ENDIF(POLICY CMP0020)
    IF(POLICY CMP0042)
        CMAKE_POLICY(SET CMP0042 NEW)
    ENDIF(POLICY CMP0042)
    IF(POLICY CMP0054)
        CMAKE_POLICY(SET CMP0054 NEW)
    ENDIF(POLICY CMP0054)
    IF(POLICY CMP0075)
        CMAKE_POLICY(SET CMP0075 NEW)
    ENDIF(POLICY CMP0075)
ENDIF(COMMAND CMAKE_POLICY)

SET(CMAKE_MODULE_PATH "${OpenAL_SOURCE_DIR}/cmake")

INCLUDE(CheckFunctionExists)
INCLUDE(CheckLibraryExists)
INCLUDE(CheckSharedFunctionExists)
INCLUDE(CheckIncludeFile)
INCLUDE(CheckIncludeFiles)
INCLUDE(CheckSymbolExists)
INCLUDE(CheckCCompilerFlag)
INCLUDE(CheckCXXCompilerFlag)
INCLUDE(CheckCSourceCompiles)
INCLUDE(CheckCXXSourceCompiles)
INCLUDE(CheckTypeSize)
include(CheckStructHasMember)
include(CheckFileOffsetBits)
include(GNUInstallDirs)

SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)


OPTION(ALSOFT_DLOPEN  "Check for the dlopen API for loading optional libs"  ON)
OPTION(ALSOFT_WERROR  "Treat compile warnings as errors"      OFF)
OPTION(ALSOFT_UTILS          "Build and install utility programs"         OFF)
OPTION(ALSOFT_NO_CONFIG_UTIL "Disable building the alsoft-config utility" OFF)
OPTION(ALSOFT_EXAMPLES  "Build and install example programs"  OFF)
OPTION(ALSOFT_TESTS     "Build and install test programs"     OFF)
OPTION(ALSOFT_CONFIG "Install alsoft.conf sample configuration file" OFF)
OPTION(ALSOFT_HRTF_DEFS "Install HRTF definition files" OFF)
OPTION(ALSOFT_AMBDEC_PRESETS "Install AmbDec preset files" OFF)
OPTION(ALSOFT_INSTALL "Install headers and libraries" OFF)

if(DEFINED SHARE_INSTALL_DIR)
    message(WARNING "SHARE_INSTALL_DIR is deprecated.  Use the variables provided by the GNUInstallDirs module instead")
    set(CMAKE_INSTALL_DATADIR "${SHARE_INSTALL_DIR}")
endif()

if(DEFINED LIB_SUFFIX)
    message(WARNING "LIB_SUFFIX is deprecated.  Use the variables provided by the GNUInstallDirs module instead")
endif()


SET(CPP_DEFS ) # C pre-process, not C++
SET(INC_PATHS )
SET(C_FLAGS )
SET(LINKER_FLAGS )
SET(EXTRA_LIBS )

IF(WIN32)
    SET(CPP_DEFS ${CPP_DEFS} _WIN32 _WIN32_WINNT=0x0502)

    OPTION(ALSOFT_BUILD_ROUTER  "Build the router (EXPERIMENTAL; creates OpenAL32.dll and soft_oal.dll)"  OFF)

    # This option is mainly for static linking OpenAL Soft into another project
    # that already defines the IDs. It is up to that project to ensure all
    # required IDs are defined.
    OPTION(ALSOFT_NO_UID_DEFS "Do not define GUIDs, IIDs, CLSIDs, or PropertyKeys" OFF)

    IF(MINGW)
        OPTION(ALSOFT_BUILD_IMPORT_LIB "Build an import .lib using dlltool (requires sed)" ON)
        IF(NOT DLLTOOL)
            IF(HOST)
                SET(DLLTOOL "${HOST}-dlltool")
            ELSE()
                SET(DLLTOOL "dlltool")
            ENDIF()
        ENDIF()
    ENDIF()
ENDIF()


# QNX's gcc do not uses /usr/include and /usr/lib pathes by default
IF ("${CMAKE_C_PLATFORM_ID}" STREQUAL "QNX")
    SET(INC_PATHS ${INC_PATHS} /usr/include)
    SET(LINKER_FLAGS ${LINKER_FLAGS} -L/usr/lib)
ENDIF()

IF(NOT LIBTYPE)
    SET(LIBTYPE SHARED)
ENDIF()

SET(LIB_MAJOR_VERSION "1")
SET(LIB_MINOR_VERSION "19")
SET(LIB_REVISION "1")
SET(LIB_VERSION "${LIB_MAJOR_VERSION}.${LIB_MINOR_VERSION}.${LIB_REVISION}")

SET(EXPORT_DECL "")
SET(ALIGN_DECL "")


CHECK_TYPE_SIZE("long" SIZEOF_LONG)
CHECK_TYPE_SIZE("long long" SIZEOF_LONG_LONG)


CHECK_C_COMPILER_FLAG(-std=c11 HAVE_STD_C11)
IF(HAVE_STD_C11)
    SET(CMAKE_C_FLAGS "-std=c11 ${CMAKE_C_FLAGS}")
ELSE()
    CHECK_C_COMPILER_FLAG(-std=c99 HAVE_STD_C99)
    IF(HAVE_STD_C99)
        SET(CMAKE_C_FLAGS "-std=c99 ${CMAKE_C_FLAGS}")
    ENDIF()
ENDIF()

CHECK_CXX_COMPILER_FLAG(-std=c++11 HAVE_STD_CXX11)
IF(HAVE_STD_CXX11)
    SET(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
ENDIF()

if(NOT WIN32)
    # Check if _POSIX_C_SOURCE and _XOPEN_SOURCE needs to be set for POSIX functions
    CHECK_SYMBOL_EXISTS(posix_memalign stdlib.h HAVE_POSIX_MEMALIGN_DEFAULT)
    IF(NOT HAVE_POSIX_MEMALIGN_DEFAULT)
        SET(OLD_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})
        SET(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -D_POSIX_C_SOURCE=200112L -D_XOPEN_SOURCE=600")
        CHECK_SYMBOL_EXISTS(posix_memalign stdlib.h HAVE_POSIX_MEMALIGN_POSIX)
        IF(NOT HAVE_POSIX_MEMALIGN_POSIX)
            SET(CMAKE_REQUIRED_FLAGS ${OLD_REQUIRED_FLAGS})
        ELSE()
            SET(CPP_DEFS ${CPP_DEFS} _POSIX_C_SOURCE=200112L _XOPEN_SOURCE=600)
        ENDIF()
    ENDIF()
    UNSET(OLD_REQUIRED_FLAGS)
ENDIF()

# Set defines for large file support
CHECK_FILE_OFFSET_BITS()
IF(_FILE_OFFSET_BITS)
    SET(CPP_DEFS ${CPP_DEFS} "_FILE_OFFSET_BITS=${_FILE_OFFSET_BITS}")
    SET(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -D_FILE_OFFSET_BITS=${_FILE_OFFSET_BITS}")
ENDIF()
SET(CPP_DEFS ${CPP_DEFS} _LARGEFILE_SOURCE _LARGE_FILES)
SET(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -D_LARGEFILE_SOURCE -D_LARGE_FILES")

# MSVC may need workarounds for C99 restrict and inline
IF(MSVC)
    # TODO: Once we truly require C99, these restrict and inline checks should go
    # away.
    CHECK_C_SOURCE_COMPILES("int *restrict foo;
                             int main() {return 0;}" HAVE_RESTRICT)
    IF(NOT HAVE_RESTRICT)
        SET(CPP_DEFS ${CPP_DEFS} "restrict=")
        SET(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -Drestrict=")
    ENDIF()

    CHECK_C_SOURCE_COMPILES("inline void foo(void) { }
                             int main() {return 0;}" HAVE_INLINE)
    IF(NOT HAVE_INLINE)
        CHECK_C_SOURCE_COMPILES("__inline void foo(void) { }
                                 int main() {return 0;}" HAVE___INLINE)
        IF(NOT HAVE___INLINE)
            MESSAGE(FATAL_ERROR "No inline keyword found, please report!")
        ENDIF()

        SET(CPP_DEFS ${CPP_DEFS} inline=__inline)
        SET(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -Dinline=__inline")
    ENDIF()
ENDIF()

# Make sure we have C99-style inline semantics with GCC (4.3 or newer).
IF(CMAKE_COMPILER_IS_GNUCC)
    SET(CMAKE_C_FLAGS "-fno-gnu89-inline ${CMAKE_C_FLAGS}")

    SET(OLD_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
    # Force no inlining for the next test.
    SET(CMAKE_REQUIRED_FLAGS "${OLD_REQUIRED_FLAGS} -fno-inline")

    CHECK_C_SOURCE_COMPILES("extern inline int foo() { return 0; }
                             int main() {return foo();}" INLINE_IS_C99)
    IF(NOT INLINE_IS_C99)
        MESSAGE(FATAL_ERROR "Your compiler does not seem to have C99 inline semantics!
                             Please update your compiler for better C99 compliance.")
    ENDIF()

    SET(CMAKE_REQUIRED_FLAGS "${OLD_REQUIRED_FLAGS}")
ENDIF()

# Check if we have a proper timespec declaration
CHECK_STRUCT_HAS_MEMBER("struct timespec" tv_sec time.h HAVE_STRUCT_TIMESPEC)
IF(HAVE_STRUCT_TIMESPEC)
    # Define it here so we don't have to include config.h for it
    SET(CPP_DEFS ${CPP_DEFS} HAVE_STRUCT_TIMESPEC)
ENDIF()

# Some systems may need libatomic for C11 atomic functions to work
SET(OLD_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES})
SET(CMAKE_REQUIRED_LIBRARIES ${OLD_REQUIRED_LIBRARIES} atomic)
CHECK_C_SOURCE_COMPILES("#include <stdatomic.h>
int _Atomic foo = ATOMIC_VAR_INIT(0);
int main()
{
    return atomic_fetch_add(&foo, 2);
}"
HAVE_LIBATOMIC)
IF(NOT HAVE_LIBATOMIC)
    SET(CMAKE_REQUIRED_LIBRARIES "${OLD_REQUIRED_LIBRARIES}")
ELSE()
    SET(EXTRA_LIBS atomic ${EXTRA_LIBS})
ENDIF()
UNSET(OLD_REQUIRED_LIBRARIES)

# Include liblog for Android logging
CHECK_LIBRARY_EXISTS(log __android_log_print "" HAVE_LIBLOG)
IF(HAVE_LIBLOG)
    SET(EXTRA_LIBS log ${EXTRA_LIBS})
    SET(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} log)
ENDIF()

# Check if we have C99 bool
CHECK_C_SOURCE_COMPILES(
"int main(int argc, char *argv[])
 {
     volatile _Bool ret;
     ret = (argc > 1) ? 1 : 0;
     return ret ? -1 : 0;
 }"
HAVE_C99_BOOL)

# Check if we have C11 static_assert
CHECK_C_SOURCE_COMPILES(
"int main()
 {
     _Static_assert(sizeof(int) == sizeof(int), \"What\");
     return 0;
 }"
HAVE_C11_STATIC_ASSERT)

# Check if we have C11 alignas
CHECK_C_SOURCE_COMPILES(
"_Alignas(16) int foo;
 int main()
 {
     return 0;
 }"
HAVE_C11_ALIGNAS)

# Check if we have C11 _Atomic
CHECK_C_SOURCE_COMPILES(
"#include <stdatomic.h>
 int _Atomic foo = ATOMIC_VAR_INIT(0);
 int main()
 {
     atomic_fetch_add(&foo, 2);
     return 0;
 }"
HAVE_C11_ATOMIC)

# Add definitions, compiler switches, etc.
INCLUDE_DIRECTORIES("${OpenAL_SOURCE_DIR}/include" "${OpenAL_SOURCE_DIR}/common" "${OpenAL_BINARY_DIR}")

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
        "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
ENDIF()
IF(NOT CMAKE_DEBUG_POSTFIX)
    SET(CMAKE_DEBUG_POSTFIX "" CACHE STRING
        "Library postfix for debug builds. Normally left blank."
        FORCE)
ENDIF()

IF(MSVC)
    SET(CPP_DEFS ${CPP_DEFS} _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_DEPRECATE)
    SET(C_FLAGS ${C_FLAGS} /wd4098)

    IF(NOT DXSDK_DIR)
        STRING(REGEX REPLACE "\\\\" "/" DXSDK_DIR "$ENV{DXSDK_DIR}")
    ELSE()
        STRING(REGEX REPLACE "\\\\" "/" DXSDK_DIR "${DXSDK_DIR}")
    ENDIF()
    IF(DXSDK_DIR)
        MESSAGE(STATUS "Using DirectX SDK directory: ${DXSDK_DIR}")
    ENDIF()

    OPTION(FORCE_STATIC_VCRT "Force /MT for static VC runtimes" OFF)
    IF(FORCE_STATIC_VCRT)
        FOREACH(flag_var
                CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
                CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
            IF(${flag_var} MATCHES "/MD")
                STRING(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
            ENDIF()
        ENDFOREACH(flag_var)
    ENDIF()
ELSE()
    SET(C_FLAGS ${C_FLAGS} -Winline -Wall)
    CHECK_C_COMPILER_FLAG(-Wextra HAVE_W_EXTRA)
    IF(HAVE_W_EXTRA)
        SET(C_FLAGS ${C_FLAGS} -Wextra)
    ENDIF()

    #IF(ALSOFT_WERROR)
       # SET(C_FLAGS ${C_FLAGS} -Werror)
    #ENDIF()

    # We want RelWithDebInfo to actually include debug stuff (define _DEBUG
    # instead of NDEBUG)
    FOREACH(flag_var  CMAKE_C_FLAGS_RELWITHDEBINFO CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        IF(${flag_var} MATCHES "-DNDEBUG")
            STRING(REGEX REPLACE "-DNDEBUG" "-D_DEBUG" ${flag_var} "${${flag_var}}")
        ENDIF()
    ENDFOREACH()

    CHECK_C_COMPILER_FLAG(-fno-math-errno HAVE_FNO_MATH_ERRNO)
    IF(HAVE_FNO_MATH_ERRNO)
        SET(C_FLAGS ${C_FLAGS} -fno-math-errno)
    ENDIF()

    CHECK_C_SOURCE_COMPILES("int foo() __attribute__((destructor));
                             int main() {return 0;}" HAVE_GCC_DESTRUCTOR)

    option(ALSOFT_STATIC_LIBGCC "Force -static-libgcc for static GCC runtimes" OFF)
    if(ALSOFT_STATIC_LIBGCC)
        set(OLD_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES})
        set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} -static-libgcc)
        check_c_source_compiles(
"#include <stdlib.h>
int main()
{
    return 0;
}"
            HAVE_STATIC_LIBGCC_SWITCH
        )
        if(HAVE_STATIC_LIBGCC_SWITCH)
            SET(LINKER_FLAGS ${LINKER_FLAGS} -static-libgcc)
        endif()
        set(CMAKE_REQUIRED_LIBRARIES ${OLD_REQUIRED_LIBRARIES})
        unset(OLD_REQUIRED_LIBRARIES)
    endif()
ENDIF()

# Set visibility/export options if available
IF(WIN32)
    SET(EXPORT_DECL "__declspec(dllexport)")
    IF(NOT MINGW)
        SET(ALIGN_DECL "__declspec(align(x))")
    ELSE()
        SET(ALIGN_DECL "__declspec(aligned(x))")
    ENDIF()
ELSE()
    SET(OLD_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
    # Yes GCC, really don't accept visibility modes you don't support
    SET(CMAKE_REQUIRED_FLAGS "${OLD_REQUIRED_FLAGS} -Wattributes -Werror")

    CHECK_C_SOURCE_COMPILES("int foo() __attribute__((visibility(\"protected\")));
                             int main() {return 0;}" HAVE_GCC_PROTECTED_VISIBILITY)
    IF(HAVE_GCC_PROTECTED_VISIBILITY)
        SET(EXPORT_DECL "__attribute__((visibility(\"protected\")))")
    ELSE()
        CHECK_C_SOURCE_COMPILES("int foo() __attribute__((visibility(\"default\")));
                                 int main() {return 0;}" HAVE_GCC_DEFAULT_VISIBILITY)
        IF(HAVE_GCC_DEFAULT_VISIBILITY)
            SET(EXPORT_DECL "__attribute__((visibility(\"default\")))")
        ENDIF()
    ENDIF()

    IF(HAVE_GCC_PROTECTED_VISIBILITY OR HAVE_GCC_DEFAULT_VISIBILITY)
        CHECK_C_COMPILER_FLAG(-fvisibility=hidden HAVE_VISIBILITY_HIDDEN_SWITCH)
        IF(HAVE_VISIBILITY_HIDDEN_SWITCH)
            SET(C_FLAGS ${C_FLAGS} -fvisibility=hidden)
        ENDIF()
    ENDIF()

    CHECK_C_SOURCE_COMPILES("int foo __attribute__((aligned(16)));
                             int main() {return 0;}" HAVE_ATTRIBUTE_ALIGNED)
    IF(HAVE_ATTRIBUTE_ALIGNED)
        SET(ALIGN_DECL "__attribute__((aligned(x)))")
    ENDIF()

    SET(CMAKE_REQUIRED_FLAGS "${OLD_REQUIRED_FLAGS}")
ENDIF()

CHECK_C_SOURCE_COMPILES("
int main()
{
    float *ptr;
    ptr = __builtin_assume_aligned(ptr, 16);
    return 0;
}" HAVE___BUILTIN_ASSUME_ALIGNED)
IF(HAVE___BUILTIN_ASSUME_ALIGNED)
    SET(ASSUME_ALIGNED_DECL "__builtin_assume_aligned(x, y)")
ELSE()
    SET(ASSUME_ALIGNED_DECL "x")
ENDIF()

SET(SSE_SWITCH "")
SET(SSE2_SWITCH "")
SET(SSE3_SWITCH "")
SET(SSE4_1_SWITCH "")
SET(FPU_NEON_SWITCH "")

CHECK_C_COMPILER_FLAG(-msse HAVE_MSSE_SWITCH)
IF(HAVE_MSSE_SWITCH)
    SET(SSE_SWITCH "-msse")
ENDIF()
CHECK_C_COMPILER_FLAG(-msse2 HAVE_MSSE2_SWITCH)
IF(HAVE_MSSE2_SWITCH)
    SET(SSE2_SWITCH "-msse2")
ENDIF()
CHECK_C_COMPILER_FLAG(-msse3 HAVE_MSSE3_SWITCH)
IF(HAVE_MSSE3_SWITCH)
    SET(SSE3_SWITCH "-msse3")
ENDIF()
CHECK_C_COMPILER_FLAG(-msse4.1 HAVE_MSSE4_1_SWITCH)
IF(HAVE_MSSE4_1_SWITCH)
    SET(SSE4_1_SWITCH "-msse4.1")
ENDIF()
CHECK_C_COMPILER_FLAG(-mfpu=neon HAVE_MFPU_NEON_SWITCH)
IF(HAVE_MFPU_NEON_SWITCH)
    SET(FPU_NEON_SWITCH "-mfpu=neon")
ENDIF()

SET(FPMATH_SET "0")
IF(CMAKE_SIZEOF_VOID_P MATCHES "4")
    IF(SSE_SWITCH OR MSVC)
        OPTION(ALSOFT_ENABLE_SSE_CODEGEN "Enable SSE code generation instead of x87 for 32-bit targets." TRUE)
    ENDIF()
    IF(SSE2_SWITCH OR MSVC)
        OPTION(ALSOFT_ENABLE_SSE2_CODEGEN "Enable SSE2 code generation instead of x87 for 32-bit targets." TRUE)
    ENDIF()

    IF(ALSOFT_ENABLE_SSE2_CODEGEN)
        IF(SSE2_SWITCH)
            CHECK_C_COMPILER_FLAG("${SSE2_SWITCH} -mfpmath=sse" HAVE_MFPMATH_SSE_2)
            IF(HAVE_MFPMATH_SSE_2)
                SET(C_FLAGS ${C_FLAGS} ${SSE2_SWITCH} -mfpmath=sse)
                SET(FPMATH_SET 2)
            ENDIF()
        ELSEIF(MSVC)
            CHECK_C_COMPILER_FLAG("/arch:SSE2" HAVE_ARCH_SSE2)
            IF(HAVE_ARCH_SSE2)
                SET(C_FLAGS ${C_FLAGS} "/arch:SSE2")
                SET(FPMATH_SET 2)
            ENDIF()
        ENDIF()
    ENDIF()
    IF(ALSOFT_ENABLE_SSE_CODEGEN AND NOT FPMATH_SET)
        IF(SSE_SWITCH)
            CHECK_C_COMPILER_FLAG("${SSE_SWITCH} -mfpmath=sse" HAVE_MFPMATH_SSE)
            IF(HAVE_MFPMATH_SSE)
                SET(C_FLAGS ${C_FLAGS} ${SSE_SWITCH} -mfpmath=sse)
                SET(FPMATH_SET 1)
            ENDIF()
        ELSEIF(MSVC)
            CHECK_C_COMPILER_FLAG("/arch:SSE" HAVE_ARCH_SSE)
            IF(HAVE_ARCH_SSE)
                SET(C_FLAGS ${C_FLAGS} "/arch:SSE")
                SET(FPMATH_SET 1)
            ENDIF()
        ENDIF()
    ENDIF()
ENDIF()


CHECK_C_SOURCE_COMPILES("int foo(const char *str, ...) __attribute__((format(printf, 1, 2)));
                         int main() {return 0;}" HAVE_GCC_FORMAT)

CHECK_INCLUDE_FILE(stdbool.h HAVE_STDBOOL_H)
CHECK_INCLUDE_FILE(stdalign.h HAVE_STDALIGN_H)
CHECK_INCLUDE_FILE(malloc.h HAVE_MALLOC_H)
CHECK_INCLUDE_FILE(dirent.h HAVE_DIRENT_H)
CHECK_INCLUDE_FILE(strings.h HAVE_STRINGS_H)
CHECK_INCLUDE_FILE(cpuid.h HAVE_CPUID_H)
CHECK_INCLUDE_FILE(intrin.h HAVE_INTRIN_H)
CHECK_INCLUDE_FILE(sys/sysconf.h HAVE_SYS_SYSCONF_H)
CHECK_INCLUDE_FILE(fenv.h HAVE_FENV_H)
CHECK_INCLUDE_FILE(float.h HAVE_FLOAT_H)
CHECK_INCLUDE_FILE(ieeefp.h HAVE_IEEEFP_H)
CHECK_INCLUDE_FILE(guiddef.h HAVE_GUIDDEF_H)
IF(NOT HAVE_GUIDDEF_H)
    CHECK_INCLUDE_FILE(initguid.h HAVE_INITGUID_H)
ENDIF()

# Some systems need libm for some of the following math functions to work
SET(MATH_LIB )
CHECK_LIBRARY_EXISTS(m pow "" HAVE_LIBM)
IF(HAVE_LIBM)
    SET(MATH_LIB ${MATH_LIB} m)
    SET(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} m)
ENDIF()

# Check for the dlopen API (for dynamicly loading backend libs)
IF(ALSOFT_DLOPEN)
    CHECK_LIBRARY_EXISTS(dl dlopen "" HAVE_LIBDL)
    IF(HAVE_LIBDL)
        SET(EXTRA_LIBS dl ${EXTRA_LIBS})
        SET(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} dl)
    ENDIF()

    CHECK_INCLUDE_FILE(dlfcn.h HAVE_DLFCN_H)
ENDIF()

# Check for a cpuid intrinsic
IF(HAVE_CPUID_H)
    CHECK_C_SOURCE_COMPILES("#include <cpuid.h>
        int main()
        {
            unsigned int eax, ebx, ecx, edx;
            return __get_cpuid(0, &eax, &ebx, &ecx, &edx);
        }" HAVE_GCC_GET_CPUID)
ENDIF()
IF(HAVE_INTRIN_H)
    CHECK_C_SOURCE_COMPILES("#include <intrin.h>
        int main()
        {
            int regs[4];
            __cpuid(regs, 0);
            return regs[0];
        }" HAVE_CPUID_INTRINSIC)
    CHECK_C_SOURCE_COMPILES("#include <intrin.h>
        int main()
        {
            unsigned long idx = 0;
            _BitScanForward64(&idx, 1);
            return idx;
        }" HAVE_BITSCANFORWARD64_INTRINSIC)
    CHECK_C_SOURCE_COMPILES("#include <intrin.h>
        int main()
        {
            unsigned long idx = 0;
            _BitScanForward(&idx, 1);
            return idx;
        }" HAVE_BITSCANFORWARD_INTRINSIC)
ENDIF()

CHECK_SYMBOL_EXISTS(sysconf          unistd.h HAVE_SYSCONF)
CHECK_SYMBOL_EXISTS(aligned_alloc    stdlib.h HAVE_ALIGNED_ALLOC)
CHECK_SYMBOL_EXISTS(posix_memalign   stdlib.h HAVE_POSIX_MEMALIGN)
CHECK_SYMBOL_EXISTS(_aligned_malloc  malloc.h HAVE__ALIGNED_MALLOC)
CHECK_SYMBOL_EXISTS(proc_pidpath     libproc.h HAVE_PROC_PIDPATH)
CHECK_SYMBOL_EXISTS(lrintf math.h HAVE_LRINTF)
CHECK_SYMBOL_EXISTS(modff  math.h HAVE_MODFF)
CHECK_SYMBOL_EXISTS(log2f  math.h HAVE_LOG2F)
CHECK_SYMBOL_EXISTS(cbrtf  math.h HAVE_CBRTF)
CHECK_SYMBOL_EXISTS(copysignf math.h HAVE_COPYSIGNF)

IF(HAVE_FLOAT_H)
    CHECK_SYMBOL_EXISTS(_controlfp float.h HAVE__CONTROLFP)
    CHECK_SYMBOL_EXISTS(__control87_2 float.h HAVE___CONTROL87_2)
ENDIF()

CHECK_FUNCTION_EXISTS(stat HAVE_STAT)
CHECK_FUNCTION_EXISTS(strtof HAVE_STRTOF)
CHECK_FUNCTION_EXISTS(strcasecmp HAVE_STRCASECMP)
IF(NOT HAVE_STRCASECMP)
    CHECK_FUNCTION_EXISTS(_stricmp HAVE__STRICMP)
    IF(NOT HAVE__STRICMP)
        MESSAGE(FATAL_ERROR "No case-insensitive compare function found, please report!")
    ENDIF()

    SET(CPP_DEFS ${CPP_DEFS} strcasecmp=_stricmp)
ENDIF()

CHECK_FUNCTION_EXISTS(strncasecmp HAVE_STRNCASECMP)
IF(NOT HAVE_STRNCASECMP)
    CHECK_FUNCTION_EXISTS(_strnicmp HAVE__STRNICMP)
    IF(NOT HAVE__STRNICMP)
        MESSAGE(FATAL_ERROR "No case-insensitive size-limitted compare function found, please report!")
    ENDIF()

    SET(CPP_DEFS ${CPP_DEFS} strncasecmp=_strnicmp)
ENDIF()

CHECK_SYMBOL_EXISTS(strnlen string.h HAVE_STRNLEN)
CHECK_SYMBOL_EXISTS(snprintf stdio.h HAVE_SNPRINTF)
IF(NOT HAVE_SNPRINTF)
    CHECK_FUNCTION_EXISTS(_snprintf HAVE__SNPRINTF)
    IF(NOT HAVE__SNPRINTF)
        MESSAGE(FATAL_ERROR "No snprintf function found, please report!")
    ENDIF()

    SET(CPP_DEFS ${CPP_DEFS} snprintf=_snprintf)
ENDIF()

CHECK_SYMBOL_EXISTS(isfinite math.h HAVE_ISFINITE)
IF(NOT HAVE_ISFINITE)
    CHECK_FUNCTION_EXISTS(finite HAVE_FINITE)
    IF(NOT HAVE_FINITE)
        CHECK_FUNCTION_EXISTS(_finite HAVE__FINITE)
        IF(NOT HAVE__FINITE)
            MESSAGE(FATAL_ERROR "No isfinite function found, please report!")
        ENDIF()
        SET(CPP_DEFS ${CPP_DEFS} isfinite=_finite)
    ELSE()
        SET(CPP_DEFS ${CPP_DEFS} isfinite=finite)
    ENDIF()
ENDIF()

CHECK_SYMBOL_EXISTS(isnan math.h HAVE_ISNAN)
IF(NOT HAVE_ISNAN)
    CHECK_FUNCTION_EXISTS(_isnan HAVE__ISNAN)
    IF(NOT HAVE__ISNAN)
        MESSAGE(FATAL_ERROR "No isnan function found, please report!")
    ENDIF()

    SET(CPP_DEFS ${CPP_DEFS} isnan=_isnan)
ENDIF()


# Check if we have Windows headers
SET(OLD_REQUIRED_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS})
SET(CMAKE_REQUIRED_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS} -D_WIN32_WINNT=0x0502)
CHECK_INCLUDE_FILE(windows.h HAVE_WINDOWS_H)
SET(CMAKE_REQUIRED_DEFINITIONS ${OLD_REQUIRED_DEFINITIONS})
UNSET(OLD_REQUIRED_DEFINITIONS)

IF(NOT HAVE_WINDOWS_H)
    CHECK_SYMBOL_EXISTS(gettimeofday sys/time.h HAVE_GETTIMEOFDAY)
    IF(NOT HAVE_GETTIMEOFDAY)
        MESSAGE(FATAL_ERROR "No timing function found!")
    ENDIF()

    CHECK_SYMBOL_EXISTS(nanosleep time.h HAVE_NANOSLEEP)
    IF(NOT HAVE_NANOSLEEP)
        MESSAGE(FATAL_ERROR "No sleep function found!")
    ENDIF()

    # We need pthreads outside of Windows
    CHECK_INCLUDE_FILE(pthread.h HAVE_PTHREAD_H)
    IF(NOT HAVE_PTHREAD_H)
        MESSAGE(FATAL_ERROR "PThreads is required for non-Windows builds!")
    ENDIF()
    # Some systems need pthread_np.h to get recursive mutexes
    CHECK_INCLUDE_FILES("pthread.h;pthread_np.h" HAVE_PTHREAD_NP_H)

    CHECK_C_COMPILER_FLAG(-pthread HAVE_PTHREAD)
    IF(HAVE_PTHREAD)
        SET(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -pthread")
        SET(C_FLAGS ${C_FLAGS} -pthread)
        SET(LINKER_FLAGS ${LINKER_FLAGS} -pthread)
    ENDIF()

    CHECK_LIBRARY_EXISTS(pthread pthread_create "" HAVE_LIBPTHREAD)
    IF(HAVE_LIBPTHREAD)
        SET(EXTRA_LIBS pthread ${EXTRA_LIBS})
    ENDIF()

    CHECK_SYMBOL_EXISTS(pthread_setschedparam pthread.h HAVE_PTHREAD_SETSCHEDPARAM)

    IF(HAVE_PTHREAD_NP_H)
        CHECK_SYMBOL_EXISTS(pthread_setname_np "pthread.h;pthread_np.h" HAVE_PTHREAD_SETNAME_NP)
        IF(NOT HAVE_PTHREAD_SETNAME_NP)
            CHECK_SYMBOL_EXISTS(pthread_set_name_np "pthread.h;pthread_np.h" HAVE_PTHREAD_SET_NAME_NP)
        ELSE()
            CHECK_C_SOURCE_COMPILES("
#include <pthread.h>
#include <pthread_np.h>
int main()
{
    pthread_setname_np(\"testname\");
    return 0;
}"
                PTHREAD_SETNAME_NP_ONE_PARAM
            )
            CHECK_C_SOURCE_COMPILES("
#include <pthread.h>
#include <pthread_np.h>
int main()
{
    pthread_setname_np(pthread_self(), \"%s\", \"testname\");
    return 0;
}"
                PTHREAD_SETNAME_NP_THREE_PARAMS
            )
        ENDIF()
        CHECK_SYMBOL_EXISTS(pthread_mutexattr_setkind_np "pthread.h;pthread_np.h" HAVE_PTHREAD_MUTEXATTR_SETKIND_NP)
    ELSE()
        CHECK_SYMBOL_EXISTS(pthread_setname_np pthread.h HAVE_PTHREAD_SETNAME_NP)
        IF(NOT HAVE_PTHREAD_SETNAME_NP)
            CHECK_SYMBOL_EXISTS(pthread_set_name_np pthread.h HAVE_PTHREAD_SET_NAME_NP)
        ELSE()
            CHECK_C_SOURCE_COMPILES("
#include <pthread.h>
int main()
{
    pthread_setname_np(\"testname\");
    return 0;
}"
                PTHREAD_SETNAME_NP_ONE_PARAM
            )
            CHECK_C_SOURCE_COMPILES("
#include <pthread.h>
int main()
{
    pthread_setname_np(pthread_self(), \"%s\", \"testname\");
    return 0;
}"
                PTHREAD_SETNAME_NP_THREE_PARAMS
            )
        ENDIF()
        CHECK_SYMBOL_EXISTS(pthread_mutexattr_setkind_np pthread.h HAVE_PTHREAD_MUTEXATTR_SETKIND_NP)
    ENDIF()

    CHECK_SYMBOL_EXISTS(pthread_mutex_timedlock pthread.h HAVE_PTHREAD_MUTEX_TIMEDLOCK)

    CHECK_LIBRARY_EXISTS(rt clock_gettime "" HAVE_LIBRT)
    IF(HAVE_LIBRT)
        SET(EXTRA_LIBS rt ${EXTRA_LIBS})
    ENDIF()
ENDIF()

CHECK_SYMBOL_EXISTS(getopt unistd.h HAVE_GETOPT)

# Check for a 64-bit type
CHECK_INCLUDE_FILE(stdint.h HAVE_STDINT_H)
IF(NOT HAVE_STDINT_H)
    IF(HAVE_WINDOWS_H)
        CHECK_C_SOURCE_COMPILES("#define _WIN32_WINNT 0x0502
                                 #include <windows.h>
                                 __int64 foo;
                                 int main() {return 0;}" HAVE___INT64)
    ENDIF()
    IF(NOT HAVE___INT64)
        IF(NOT SIZEOF_LONG MATCHES "8")
            IF(NOT SIZEOF_LONG_LONG MATCHES "8")
                MESSAGE(FATAL_ERROR "No 64-bit types found, please report!")
            ENDIF()
        ENDIF()
    ENDIF()
ENDIF()


SET(COMMON_OBJS
    ${OpenAL_SOURCE_DIR}/common/alcomplex.c
    ${OpenAL_SOURCE_DIR}/common/alcomplex.h
    ${OpenAL_SOURCE_DIR}/common/align.h
    ${OpenAL_SOURCE_DIR}/common/almalloc.c
    ${OpenAL_SOURCE_DIR}/common/almalloc.h
    ${OpenAL_SOURCE_DIR}/common/atomic.c
    ${OpenAL_SOURCE_DIR}/common/atomic.h
    ${OpenAL_SOURCE_DIR}/common/bool.h
    ${OpenAL_SOURCE_DIR}/common/math_defs.h
    ${OpenAL_SOURCE_DIR}/common/rwlock.c
    ${OpenAL_SOURCE_DIR}/common/rwlock.h
    ${OpenAL_SOURCE_DIR}/common/static_assert.h
    ${OpenAL_SOURCE_DIR}/common/threads.c
    ${OpenAL_SOURCE_DIR}/common/threads.h
    ${OpenAL_SOURCE_DIR}/common/uintmap.c
    ${OpenAL_SOURCE_DIR}/common/uintmap.h
)
SET(OPENAL_OBJS
    ${OpenAL_SOURCE_DIR}/OpenAL32/Include/bs2b.h
    ${OpenAL_SOURCE_DIR}/OpenAL32/Include/alMain.h
    ${OpenAL_SOURCE_DIR}/OpenAL32/Include/alu.h

    ${OpenAL_SOURCE_DIR}/OpenAL32/Include/alAuxEffectSlot.h
    ${OpenAL_SOURCE_DIR}/OpenAL32/alAuxEffectSlot.c
    ${OpenAL_SOURCE_DIR}/OpenAL32/Include/alBuffer.h
    ${OpenAL_SOURCE_DIR}/OpenAL32/alBuffer.c
    ${OpenAL_SOURCE_DIR}/OpenAL32/Include/alEffect.h
    ${OpenAL_SOURCE_DIR}/OpenAL32/alEffect.c
    ${OpenAL_SOURCE_DIR}/OpenAL32/Include/alError.h
    ${OpenAL_SOURCE_DIR}/OpenAL32/alError.c
    ${OpenAL_SOURCE_DIR}/OpenAL32/alExtension.c
    ${OpenAL_SOURCE_DIR}/OpenAL32/Include/alFilter.h
    ${OpenAL_SOURCE_DIR}/OpenAL32/alFilter.c
    ${OpenAL_SOURCE_DIR}/OpenAL32/Include/alListener.h
    ${OpenAL_SOURCE_DIR}/OpenAL32/alListener.c
    ${OpenAL_SOURCE_DIR}/OpenAL32/Include/alSource.h
    ${OpenAL_SOURCE_DIR}/OpenAL32/alSource.c
    ${OpenAL_SOURCE_DIR}/OpenAL32/alState.c
    ${OpenAL_SOURCE_DIR}/OpenAL32/event.c
    ${OpenAL_SOURCE_DIR}/OpenAL32/Include/sample_cvt.h
    ${OpenAL_SOURCE_DIR}/OpenAL32/sample_cvt.c
)
SET(ALC_OBJS
    ${OpenAL_SOURCE_DIR}/Alc/ALc.c
    ${OpenAL_SOURCE_DIR}/Alc/ALu.c
    ${OpenAL_SOURCE_DIR}/Alc/alconfig.c
    ${OpenAL_SOURCE_DIR}/Alc/alconfig.h
    ${OpenAL_SOURCE_DIR}/Alc/bs2b.c
    ${OpenAL_SOURCE_DIR}/Alc/converter.c
    ${OpenAL_SOURCE_DIR}/Alc/converter.h
    ${OpenAL_SOURCE_DIR}/Alc/inprogext.h
    ${OpenAL_SOURCE_DIR}/Alc/mastering.c
    ${OpenAL_SOURCE_DIR}/Alc/mastering.h
    ${OpenAL_SOURCE_DIR}/Alc/ringbuffer.c
    ${OpenAL_SOURCE_DIR}/Alc/ringbuffer.h
    ${OpenAL_SOURCE_DIR}/Alc/effects/autowah.c
    ${OpenAL_SOURCE_DIR}/Alc/effects/chorus.c
    ${OpenAL_SOURCE_DIR}/Alc/effects/compressor.c
    ${OpenAL_SOURCE_DIR}/Alc/effects/dedicated.c
    ${OpenAL_SOURCE_DIR}/Alc/effects/distortion.c
    ${OpenAL_SOURCE_DIR}/Alc/effects/echo.c
    ${OpenAL_SOURCE_DIR}/Alc/effects/equalizer.c
    ${OpenAL_SOURCE_DIR}/Alc/effects/fshifter.c
    ${OpenAL_SOURCE_DIR}/Alc/effects/modulator.c
    ${OpenAL_SOURCE_DIR}/Alc/effects/null.c
    ${OpenAL_SOURCE_DIR}/Alc/effects/pshifter.c
    ${OpenAL_SOURCE_DIR}/Alc/effects/reverb.c
    ${OpenAL_SOURCE_DIR}/Alc/filters/defs.h
    ${OpenAL_SOURCE_DIR}/Alc/filters/filter.c
    ${OpenAL_SOURCE_DIR}/Alc/filters/nfc.c
    ${OpenAL_SOURCE_DIR}/Alc/filters/nfc.h
    ${OpenAL_SOURCE_DIR}/Alc/filters/splitter.c
    ${OpenAL_SOURCE_DIR}/Alc/filters/splitter.h
    ${OpenAL_SOURCE_DIR}/Alc/helpers.c
    ${OpenAL_SOURCE_DIR}/Alc/alstring.h
    ${OpenAL_SOURCE_DIR}/Alc/compat.h
    ${OpenAL_SOURCE_DIR}/Alc/cpu_caps.h
    ${OpenAL_SOURCE_DIR}/Alc/fpu_modes.h
    ${OpenAL_SOURCE_DIR}/Alc/logging.h
    ${OpenAL_SOURCE_DIR}/Alc/vector.h
    ${OpenAL_SOURCE_DIR}/Alc/hrtf.c
    ${OpenAL_SOURCE_DIR}/Alc/hrtf.h
    ${OpenAL_SOURCE_DIR}/Alc/uhjfilter.c
    ${OpenAL_SOURCE_DIR}/Alc/uhjfilter.h
    ${OpenAL_SOURCE_DIR}/Alc/ambdec.c
    ${OpenAL_SOURCE_DIR}/Alc/ambdec.h
    ${OpenAL_SOURCE_DIR}/Alc/bformatdec.c
    ${OpenAL_SOURCE_DIR}/Alc/bformatdec.h
    ${OpenAL_SOURCE_DIR}/Alc/panning.c
    ${OpenAL_SOURCE_DIR}/Alc/polymorphism.h
    ${OpenAL_SOURCE_DIR}/Alc/mixvoice.c
    ${OpenAL_SOURCE_DIR}/Alc/mixer/defs.h
    ${OpenAL_SOURCE_DIR}/Alc/mixer/mixer_c.c
)


SET(CPU_EXTS "Default")
SET(HAVE_SSE        0)
SET(HAVE_SSE2       0)
SET(HAVE_SSE3       0)
SET(HAVE_SSE4_1     0)
SET(HAVE_NEON       0)

SET(HAVE_ALSA       0)
SET(HAVE_OSS        0)
SET(HAVE_SOLARIS    0)
SET(HAVE_SNDIO      0)
SET(HAVE_QSA        0)
SET(HAVE_DSOUND     0)
SET(HAVE_WASAPI     0)
SET(HAVE_WINMM      0)
SET(HAVE_PORTAUDIO  0)
SET(HAVE_PULSEAUDIO 0)
SET(HAVE_COREAUDIO  0)
SET(HAVE_OPENSL     0)
SET(HAVE_WAVE       0)
SET(HAVE_SDL2       0)

# Check for SSE support
OPTION(ALSOFT_REQUIRE_SSE "Require SSE support" OFF)
CHECK_INCLUDE_FILE(xmmintrin.h HAVE_XMMINTRIN_H "${SSE_SWITCH}")
IF(HAVE_XMMINTRIN_H)
    OPTION(ALSOFT_CPUEXT_SSE "Enable SSE support" ON)
    IF(ALSOFT_CPUEXT_SSE)
        IF(ALIGN_DECL OR HAVE_C11_ALIGNAS)
            SET(HAVE_SSE 1)
            SET(ALC_OBJS  ${ALC_OBJS} ${OpenAL_SOURCE_DIR}/Alc/mixer/mixer_sse.c)
            IF(SSE_SWITCH)
                SET_SOURCE_FILES_PROPERTIES(${OpenAL_SOURCE_DIR}/Alc/mixer/mixer_sse.c PROPERTIES
                                            COMPILE_FLAGS "${SSE_SWITCH}")
            ENDIF()
            SET(CPU_EXTS "${CPU_EXTS}, SSE")
        ENDIF()
    ENDIF()
ENDIF()
IF(ALSOFT_REQUIRE_SSE AND NOT HAVE_SSE)
    MESSAGE(FATAL_ERROR "Failed to enabled required SSE CPU extensions")
ENDIF()

OPTION(ALSOFT_REQUIRE_SSE2 "Require SSE2 support" OFF)
CHECK_INCLUDE_FILE(emmintrin.h HAVE_EMMINTRIN_H "${SSE2_SWITCH}")
IF(HAVE_EMMINTRIN_H)
    OPTION(ALSOFT_CPUEXT_SSE2 "Enable SSE2 support" ON)
    IF(HAVE_SSE AND ALSOFT_CPUEXT_SSE2)
        IF(ALIGN_DECL OR HAVE_C11_ALIGNAS)
            SET(HAVE_SSE2 1)
            SET(ALC_OBJS  ${ALC_OBJS} ${OpenAL_SOURCE_DIR}/Alc/mixer/mixer_sse2.c)
            IF(SSE2_SWITCH)
                SET_SOURCE_FILES_PROPERTIES(${OpenAL_SOURCE_DIR}/Alc/mixer/mixer_sse2.c PROPERTIES
                                            COMPILE_FLAGS "${SSE2_SWITCH}")
            ENDIF()
            SET(CPU_EXTS "${CPU_EXTS}, SSE2")
        ENDIF()
    ENDIF()
ENDIF()
IF(ALSOFT_REQUIRE_SSE2 AND NOT HAVE_SSE2)
    MESSAGE(FATAL_ERROR "Failed to enable required SSE2 CPU extensions")
ENDIF()

OPTION(ALSOFT_REQUIRE_SSE3 "Require SSE3 support" OFF)
CHECK_INCLUDE_FILE(pmmintrin.h HAVE_PMMINTRIN_H "${SSE3_SWITCH}")
IF(HAVE_EMMINTRIN_H)
    OPTION(ALSOFT_CPUEXT_SSE3 "Enable SSE3 support" ON)
    IF(HAVE_SSE2 AND ALSOFT_CPUEXT_SSE3)
        IF(ALIGN_DECL OR HAVE_C11_ALIGNAS)
            SET(HAVE_SSE3 1)
            SET(ALC_OBJS  ${ALC_OBJS} ${OpenAL_SOURCE_DIR}/Alc/mixer/mixer_sse3.c)
            IF(SSE2_SWITCH)
                SET_SOURCE_FILES_PROPERTIES(${OpenAL_SOURCE_DIR}/Alc/mixer/mixer_sse3.c PROPERTIES
                                            COMPILE_FLAGS "${SSE3_SWITCH}")
            ENDIF()
            SET(CPU_EXTS "${CPU_EXTS}, SSE3")
        ENDIF()
    ENDIF()
ENDIF()
IF(ALSOFT_REQUIRE_SSE3 AND NOT HAVE_SSE3)
    MESSAGE(FATAL_ERROR "Failed to enable required SSE3 CPU extensions")
ENDIF()

OPTION(ALSOFT_REQUIRE_SSE4_1 "Require SSE4.1 support" OFF)
CHECK_INCLUDE_FILE(smmintrin.h HAVE_SMMINTRIN_H "${SSE4_1_SWITCH}")
IF(HAVE_SMMINTRIN_H)
    OPTION(ALSOFT_CPUEXT_SSE4_1 "Enable SSE4.1 support" ON)
    IF(HAVE_SSE2 AND ALSOFT_CPUEXT_SSE4_1)
        IF(ALIGN_DECL OR HAVE_C11_ALIGNAS)
            SET(HAVE_SSE4_1 1)
            SET(ALC_OBJS  ${ALC_OBJS} ${OpenAL_SOURCE_DIR}/Alc/mixer/mixer_sse41.c)
            IF(SSE4_1_SWITCH)
                SET_SOURCE_FILES_PROPERTIES(${OpenAL_SOURCE_DIR}/Alc/mixer/mixer_sse41.c PROPERTIES
                                            COMPILE_FLAGS "${SSE4_1_SWITCH}")
            ENDIF()
            SET(CPU_EXTS "${CPU_EXTS}, SSE4.1")
        ENDIF()
    ENDIF()
ENDIF()
IF(ALSOFT_REQUIRE_SSE4_1 AND NOT HAVE_SSE4_1)
    MESSAGE(FATAL_ERROR "Failed to enable required SSE4.1 CPU extensions")
ENDIF()

# Check for ARM Neon support
OPTION(ALSOFT_REQUIRE_NEON "Require ARM Neon support" OFF)
CHECK_INCLUDE_FILE(arm_neon.h HAVE_ARM_NEON_H ${FPU_NEON_SWITCH})
IF(HAVE_ARM_NEON_H)
    OPTION(ALSOFT_CPUEXT_NEON "Enable ARM Neon support" ON)
    IF(ALSOFT_CPUEXT_NEON)
        SET(HAVE_NEON 1)
        SET(ALC_OBJS  ${ALC_OBJS} ${OpenAL_SOURCE_DIR}/Alc/mixer/mixer_neon.c)
        IF(FPU_NEON_SWITCH)
            SET_SOURCE_FILES_PROPERTIES(${OpenAL_SOURCE_DIR}/Alc/mixer/mixer_neon.c PROPERTIES
                                        COMPILE_FLAGS "${FPU_NEON_SWITCH}")
        ENDIF()
        SET(CPU_EXTS "${CPU_EXTS}, Neon")
    ENDIF()
ENDIF()
IF(ALSOFT_REQUIRE_NEON AND NOT HAVE_NEON)
    MESSAGE(FATAL_ERROR "Failed to enabled required ARM Neon CPU extensions")
ENDIF()


IF(WIN32 OR HAVE_DLFCN_H)
    SET(IS_LINKED "")
    MACRO(ADD_BACKEND_LIBS _LIBS)
    ENDMACRO()
ELSE()
    SET(IS_LINKED " (linked)")
    MACRO(ADD_BACKEND_LIBS _LIBS)
        SET(EXTRA_LIBS ${_LIBS} ${EXTRA_LIBS})
    ENDMACRO()
ENDIF()

SET(BACKENDS "")
SET(ALC_OBJS  ${ALC_OBJS}
    ${OpenAL_SOURCE_DIR}/Alc/backends/base.c
    ${OpenAL_SOURCE_DIR}/Alc/backends/base.h
    # Default backends, always available
    ${OpenAL_SOURCE_DIR}/Alc/backends/loopback.c
    ${OpenAL_SOURCE_DIR}/Alc/backends/null.c
)

# Check ALSA backend
OPTION(ALSOFT_REQUIRE_ALSA "Require ALSA backend" OFF)
FIND_PACKAGE(ALSA)
IF(ALSA_FOUND)
    OPTION(ALSOFT_BACKEND_ALSA "Enable ALSA backend" ON)
    IF(ALSOFT_BACKEND_ALSA)
        SET(HAVE_ALSA 1)
        SET(BACKENDS  "${BACKENDS} ALSA${IS_LINKED},")
        SET(ALC_OBJS  ${ALC_OBJS} ${OpenAL_SOURCE_DIR}/Alc/backends/alsa.c)
        ADD_BACKEND_LIBS(${ALSA_LIBRARIES})
        SET(INC_PATHS ${INC_PATHS} ${ALSA_INCLUDE_DIRS})
    ENDIF()
ENDIF()
IF(ALSOFT_REQUIRE_ALSA AND NOT HAVE_ALSA)
    MESSAGE(FATAL_ERROR "Failed to enabled required ALSA backend")
ENDIF()

# Check OSS backend
OPTION(ALSOFT_REQUIRE_OSS "Require OSS backend" OFF)
FIND_PACKAGE(OSS)
IF(OSS_FOUND)
    OPTION(ALSOFT_BACKEND_OSS "Enable OSS backend" ON)
    IF(ALSOFT_BACKEND_OSS)
        SET(HAVE_OSS 1)
        SET(BACKENDS  "${BACKENDS} OSS,")
        SET(ALC_OBJS  ${ALC_OBJS} ${OpenAL_SOURCE_DIR}/Alc/backends/oss.c)
        IF(OSS_LIBRARIES)
            SET(EXTRA_LIBS ${OSS_LIBRARIES} ${EXTRA_LIBS})
        ENDIF()
        SET(INC_PATHS ${INC_PATHS} ${OSS_INCLUDE_DIRS})
    ENDIF()
ENDIF()
IF(ALSOFT_REQUIRE_OSS AND NOT HAVE_OSS)
    MESSAGE(FATAL_ERROR "Failed to enabled required OSS backend")
ENDIF()

# Check Solaris backend
OPTION(ALSOFT_REQUIRE_SOLARIS "Require Solaris backend" OFF)
FIND_PACKAGE(AudioIO)
IF(AUDIOIO_FOUND)
    OPTION(ALSOFT_BACKEND_SOLARIS "Enable Solaris backend" ON)
    IF(ALSOFT_BACKEND_SOLARIS)
        SET(HAVE_SOLARIS 1)
        SET(BACKENDS  "${BACKENDS} Solaris,")
        SET(ALC_OBJS  ${ALC_OBJS} ${OpenAL_SOURCE_DIR}/Alc/backends/solaris.c)
        SET(INC_PATHS ${INC_PATHS} ${AUDIOIO_INCLUDE_DIRS})
    ENDIF()
ENDIF()
IF(ALSOFT_REQUIRE_SOLARIS AND NOT HAVE_SOLARIS)
    MESSAGE(FATAL_ERROR "Failed to enabled required Solaris backend")
ENDIF()

# Check SndIO backend
OPTION(ALSOFT_REQUIRE_SNDIO "Require SndIO backend" OFF)
FIND_PACKAGE(SoundIO)
IF(SOUNDIO_FOUND)
    OPTION(ALSOFT_BACKEND_SNDIO "Enable SndIO backend" ON)
    IF(ALSOFT_BACKEND_SNDIO)
        SET(HAVE_SNDIO 1)
        SET(BACKENDS  "${BACKENDS} SndIO (linked),")
        SET(ALC_OBJS  ${ALC_OBJS} ${OpenAL_SOURCE_DIR}/Alc/backends/sndio.c)
        SET(EXTRA_LIBS ${SOUNDIO_LIBRARIES} ${EXTRA_LIBS})
        SET(INC_PATHS ${INC_PATHS} ${SOUNDIO_INCLUDE_DIRS})
    ENDIF()
ENDIF()
IF(ALSOFT_REQUIRE_SNDIO AND NOT HAVE_SNDIO)
    MESSAGE(FATAL_ERROR "Failed to enabled required SndIO backend")
ENDIF()

# Check QSA backend
OPTION(ALSOFT_REQUIRE_QSA "Require QSA backend" OFF)
FIND_PACKAGE(QSA)
IF(QSA_FOUND)
    OPTION(ALSOFT_BACKEND_QSA "Enable QSA backend" ON)
    IF(ALSOFT_BACKEND_QSA)
        SET(HAVE_QSA 1)
        SET(BACKENDS  "${BACKENDS} QSA (linked),")
        SET(ALC_OBJS  ${ALC_OBJS} ${OpenAL_SOURCE_DIR}/Alc/backends/qsa.c)
        SET(EXTRA_LIBS ${QSA_LIBRARIES} ${EXTRA_LIBS})
        SET(INC_PATHS ${INC_PATHS} ${QSA_INCLUDE_DIRS})
    ENDIF()
ENDIF()
IF(ALSOFT_REQUIRE_QSA AND NOT HAVE_QSA)
    MESSAGE(FATAL_ERROR "Failed to enabled required QSA backend")
ENDIF()

# Check Windows-only backends
OPTION(ALSOFT_REQUIRE_WINMM "Require Windows Multimedia backend" OFF)
OPTION(ALSOFT_REQUIRE_DSOUND "Require DirectSound backend" OFF)
OPTION(ALSOFT_REQUIRE_WASAPI "Require WASAPI backend" OFF)
IF(HAVE_WINDOWS_H)
    SET(OLD_REQUIRED_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS})
    SET(CMAKE_REQUIRED_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS} -D_WIN32_WINNT=0x0502)

    # Check MMSystem backend
    CHECK_INCLUDE_FILES("windows.h;mmsystem.h" HAVE_MMSYSTEM_H)
    IF(HAVE_MMSYSTEM_H)
        CHECK_SHARED_FUNCTION_EXISTS(waveOutOpen "windows.h;mmsystem.h" winmm "" HAVE_LIBWINMM)
        IF(HAVE_LIBWINMM)
            OPTION(ALSOFT_BACKEND_WINMM "Enable Windows Multimedia backend" ON)
            IF(ALSOFT_BACKEND_WINMM)
                SET(HAVE_WINMM 1)
                SET(BACKENDS  "${BACKENDS} WinMM,")
                SET(ALC_OBJS  ${ALC_OBJS} ${OpenAL_SOURCE_DIR}/Alc/backends/winmm.c)
                SET(EXTRA_LIBS winmm ${EXTRA_LIBS})
            ENDIF()
        ENDIF()
    ENDIF()

    # Check DSound backend
    FIND_PACKAGE(DSound)
    IF(DSOUND_FOUND)
        OPTION(ALSOFT_BACKEND_DSOUND "Enable DirectSound backend" ON)
        IF(ALSOFT_BACKEND_DSOUND)
            SET(HAVE_DSOUND 1)
            SET(BACKENDS  "${BACKENDS} DirectSound${IS_LINKED},")
            SET(ALC_OBJS  ${ALC_OBJS} ${OpenAL_SOURCE_DIR}/Alc/backends/dsound.c)
            ADD_BACKEND_LIBS(${DSOUND_LIBRARIES})
            SET(INC_PATHS ${INC_PATHS} ${DSOUND_INCLUDE_DIRS})
        ENDIF()
    ENDIF()

    # Check for WASAPI backend
    CHECK_INCLUDE_FILE(mmdeviceapi.h HAVE_MMDEVICEAPI_H)
    IF(HAVE_MMDEVICEAPI_H)
        OPTION(ALSOFT_BACKEND_WASAPI "Enable WASAPI backend" ON)
        IF(ALSOFT_BACKEND_WASAPI)
            SET(HAVE_WASAPI 1)
            SET(BACKENDS  "${BACKENDS} WASAPI,")
            SET(ALC_OBJS  ${ALC_OBJS} ${OpenAL_SOURCE_DIR}/Alc/backends/wasapi.c)
        ENDIF()
    ENDIF()

    SET(CMAKE_REQUIRED_DEFINITIONS ${OLD_REQUIRED_DEFINITIONS})
    UNSET(OLD_REQUIRED_DEFINITIONS)
ENDIF()
IF(ALSOFT_REQUIRE_WINMM AND NOT HAVE_WINMM)
    MESSAGE(FATAL_ERROR "Failed to enabled required WinMM backend")
ENDIF()
IF(ALSOFT_REQUIRE_DSOUND AND NOT HAVE_DSOUND)
    MESSAGE(FATAL_ERROR "Failed to enabled required DSound backend")
ENDIF()
IF(ALSOFT_REQUIRE_WASAPI AND NOT HAVE_WASAPI)
    MESSAGE(FATAL_ERROR "Failed to enabled required WASAPI backend")
ENDIF()

# Check PortAudio backend
OPTION(ALSOFT_REQUIRE_PORTAUDIO "Require PortAudio backend" OFF)
FIND_PACKAGE(PortAudio)
IF(PORTAUDIO_FOUND)
    OPTION(ALSOFT_BACKEND_PORTAUDIO "Enable PortAudio backend" ON)
    IF(ALSOFT_BACKEND_PORTAUDIO)
        SET(HAVE_PORTAUDIO 1)
        SET(BACKENDS  "${BACKENDS} PortAudio${IS_LINKED},")
        SET(ALC_OBJS  ${ALC_OBJS} ${OpenAL_SOURCE_DIR}/Alc/backends/portaudio.c)
        ADD_BACKEND_LIBS(${PORTAUDIO_LIBRARIES})
        SET(INC_PATHS ${INC_PATHS} ${PORTAUDIO_INCLUDE_DIRS})
    ENDIF()
ENDIF()
IF(ALSOFT_REQUIRE_PORTAUDIO AND NOT HAVE_PORTAUDIO)
    MESSAGE(FATAL_ERROR "Failed to enabled required PortAudio backend")
ENDIF()

# Check PulseAudio backend
OPTION(ALSOFT_REQUIRE_PULSEAUDIO "Require PulseAudio backend" OFF)
FIND_PACKAGE(PulseAudio)
IF(PULSEAUDIO_FOUND)
    OPTION(ALSOFT_BACKEND_PULSEAUDIO "Enable PulseAudio backend" ON)
    IF(ALSOFT_BACKEND_PULSEAUDIO)
        SET(HAVE_PULSEAUDIO 1)
        SET(BACKENDS  "${BACKENDS} PulseAudio${IS_LINKED},")
        SET(ALC_OBJS  ${ALC_OBJS} ${OpenAL_SOURCE_DIR}/Alc/backends/pulseaudio.c)
        ADD_BACKEND_LIBS(${PULSEAUDIO_LIBRARIES})
        SET(INC_PATHS ${INC_PATHS} ${PULSEAUDIO_INCLUDE_DIRS})
    ENDIF()
ENDIF()
IF(ALSOFT_REQUIRE_PULSEAUDIO AND NOT HAVE_PULSEAUDIO)
    MESSAGE(FATAL_ERROR "Failed to enabled required PulseAudio backend")
ENDIF()

# Check JACK backend
OPTION(ALSOFT_REQUIRE_JACK "Require JACK backend" OFF)
FIND_PACKAGE(JACK)
IF(JACK_FOUND)
    OPTION(ALSOFT_BACKEND_JACK "Enable JACK backend" ON)
    IF(ALSOFT_BACKEND_JACK)
        SET(HAVE_JACK 1)
        SET(BACKENDS  "${BACKENDS} JACK${IS_LINKED},")
        SET(ALC_OBJS  ${ALC_OBJS} ${OpenAL_SOURCE_DIR}/Alc/backends/jack.c)
        ADD_BACKEND_LIBS(${JACK_LIBRARIES})
        SET(INC_PATHS ${INC_PATHS} ${JACK_INCLUDE_DIRS})
    ENDIF()
ENDIF()
IF(ALSOFT_REQUIRE_JACK AND NOT HAVE_JACK)
    MESSAGE(FATAL_ERROR "Failed to enabled required JACK backend")
ENDIF()

# Check CoreAudio backend
OPTION(ALSOFT_REQUIRE_COREAUDIO "Require CoreAudio backend" OFF)
FIND_LIBRARY(COREAUDIO_FRAMEWORK
             NAMES CoreAudio
             PATHS /System/Library/Frameworks
)
IF(COREAUDIO_FRAMEWORK)
    OPTION(ALSOFT_BACKEND_COREAUDIO "Enable CoreAudio backend" ON)
    IF(ALSOFT_BACKEND_COREAUDIO)
        SET(HAVE_COREAUDIO 1)
        SET(ALC_OBJS  ${ALC_OBJS} ${OpenAL_SOURCE_DIR}/Alc/backends/coreaudio.c)
        SET(BACKENDS  "${BACKENDS} CoreAudio,")
        SET(EXTRA_LIBS ${COREAUDIO_FRAMEWORK} ${EXTRA_LIBS})
        SET(EXTRA_LIBS /System/Library/Frameworks/AudioUnit.framework ${EXTRA_LIBS})
        SET(EXTRA_LIBS /System/Library/Frameworks/ApplicationServices.framework ${EXTRA_LIBS})

        # Some versions of OSX may need the AudioToolbox framework. Add it if
        # it's found.
        FIND_LIBRARY(AUDIOTOOLBOX_LIBRARY
                     NAMES AudioToolbox
                     PATHS ~/Library/Frameworks
                           /Library/Frameworks
                           /System/Library/Frameworks
                    )
        IF(AUDIOTOOLBOX_LIBRARY)
            SET(EXTRA_LIBS ${AUDIOTOOLBOX_LIBRARY} ${EXTRA_LIBS})
        ENDIF()
    ENDIF()
ENDIF()
IF(ALSOFT_REQUIRE_COREAUDIO AND NOT HAVE_COREAUDIO)
    MESSAGE(FATAL_ERROR "Failed to enabled required CoreAudio backend")
ENDIF()

# Check for OpenSL (Android) backend
OPTION(ALSOFT_REQUIRE_OPENSL "Require OpenSL backend" OFF)
CHECK_INCLUDE_FILES("SLES/OpenSLES.h;SLES/OpenSLES_Android.h" HAVE_SLES_OPENSLES_ANDROID_H)
IF(HAVE_SLES_OPENSLES_ANDROID_H)
    CHECK_SHARED_FUNCTION_EXISTS(slCreateEngine "SLES/OpenSLES.h" OpenSLES "" HAVE_LIBOPENSLES)
    IF(HAVE_LIBOPENSLES)
        OPTION(ALSOFT_BACKEND_OPENSL "Enable OpenSL backend" ON)
        IF(ALSOFT_BACKEND_OPENSL)
            SET(HAVE_OPENSL 1)
            SET(ALC_OBJS  ${ALC_OBJS} ${OpenAL_SOURCE_DIR}/Alc/backends/opensl.c)
            SET(BACKENDS  "${BACKENDS} OpenSL,")
            SET(EXTRA_LIBS OpenSLES ${EXTRA_LIBS})
        ENDIF()
    ENDIF()
ENDIF()
IF(ALSOFT_REQUIRE_OPENSL AND NOT HAVE_OPENSL)
    MESSAGE(FATAL_ERROR "Failed to enabled required OpenSL backend")
ENDIF()

# Check for SDL2 backend
OPTION(ALSOFT_REQUIRE_SDL2 "Require SDL2 backend" OFF)
FIND_PACKAGE(SDL2)
IF(SDL2_FOUND)
    # Off by default, since it adds a runtime dependency
    OPTION(ALSOFT_BACKEND_SDL2 "Enable SDL2 backend" OFF)
    IF(ALSOFT_BACKEND_SDL2)
        SET(HAVE_SDL2 1)
        SET(ALC_OBJS  ${ALC_OBJS} ${OpenAL_SOURCE_DIR}/Alc/backends/sdl2.c)
        SET(BACKENDS  "${BACKENDS} SDL2,")
        SET(EXTRA_LIBS ${SDL2_LIBRARY} ${EXTRA_LIBS})
        SET(INC_PATHS ${INC_PATHS} ${SDL2_INCLUDE_DIR})
    ENDIF()
ENDIF()
IF(ALSOFT_REQUIRE_SDL2 AND NOT SDL2_FOUND)
    MESSAGE(FATAL_ERROR "Failed to enabled required SDL2 backend")
ENDIF()

# Optionally enable the Wave Writer backend
OPTION(ALSOFT_BACKEND_WAVE "Enable Wave Writer backend" ON)
IF(ALSOFT_BACKEND_WAVE)
    SET(HAVE_WAVE 1)
    SET(ALC_OBJS  ${ALC_OBJS} ${OpenAL_SOURCE_DIR}/Alc/backends/wave.c)
    SET(BACKENDS  "${BACKENDS} WaveFile,")
ENDIF()

# This is always available
SET(BACKENDS  "${BACKENDS} Null")


FIND_PACKAGE(Git)
IF(GIT_FOUND AND EXISTS "${OpenAL_SOURCE_DIR}/.git")
    # Get the current working branch and its latest abbreviated commit hash
    ADD_CUSTOM_TARGET(build_version
        ${CMAKE_COMMAND} -D GIT_EXECUTABLE=${GIT_EXECUTABLE}
                         -D LIB_VERSION=${LIB_VERSION}
                         -D SRC=${OpenAL_SOURCE_DIR}/version.h.in
                         -D DST=${OpenAL_BINARY_DIR}/version.h
                         -P ${OpenAL_SOURCE_DIR}/version.cmake
        WORKING_DIRECTORY "${OpenAL_SOURCE_DIR}"
        VERBATIM
    )
ELSE()
    SET(GIT_BRANCH "UNKNOWN")
    SET(GIT_COMMIT_HASH "unknown")
    CONFIGURE_FILE(
        "${OpenAL_SOURCE_DIR}/version.h.in"
        "${OpenAL_BINARY_DIR}/version.h")
ENDIF()

SET(NATIVE_SRC_DIR  "${OpenAL_SOURCE_DIR}/native-tools/")
SET(NATIVE_BIN_DIR  "${OpenAL_BINARY_DIR}/native-tools/")
FILE(MAKE_DIRECTORY "${NATIVE_BIN_DIR}")

SET(BIN2H_COMMAND  "${NATIVE_BIN_DIR}bin2h")
SET(BSINCGEN_COMMAND  "${NATIVE_BIN_DIR}bsincgen")
ADD_CUSTOM_COMMAND(OUTPUT "${BIN2H_COMMAND}" "${BSINCGEN_COMMAND}"
    COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" "${NATIVE_SRC_DIR}"
    COMMAND ${CMAKE_COMMAND} -E remove "${BIN2H_COMMAND}" "${BSINCGEN_COMMAND}"
    COMMAND ${CMAKE_COMMAND} --build . --config "Release"
    WORKING_DIRECTORY "${NATIVE_BIN_DIR}"
    DEPENDS "${NATIVE_SRC_DIR}CMakeLists.txt"
    IMPLICIT_DEPENDS C "${NATIVE_SRC_DIR}bin2h.c"
                     C "${NATIVE_SRC_DIR}bsincgen.c"
    VERBATIM
)
ADD_CUSTOM_TARGET(native-tools
    DEPENDS "${BIN2H_COMMAND}" "${BSINCGEN_COMMAND}"
    VERBATIM
)

option(ALSOFT_EMBED_HRTF_DATA "Embed the HRTF data files (increases library footprint)" ON)
if(ALSOFT_EMBED_HRTF_DATA)
    MACRO(make_hrtf_header FILENAME VARNAME)
        SET(infile  "${OpenAL_SOURCE_DIR}/hrtf/${FILENAME}")
        SET(outfile  "${OpenAL_BINARY_DIR}/${FILENAME}.h")

        ADD_CUSTOM_COMMAND(OUTPUT "${outfile}"
            COMMAND "${BIN2H_COMMAND}" "${infile}" "${outfile}" ${VARNAME}
            DEPENDS native-tools "${infile}"
            VERBATIM
        )
        SET(ALC_OBJS  ${ALC_OBJS} "${outfile}")
    ENDMACRO()

    make_hrtf_header(default-44100.mhr "hrtf_default_44100")
    make_hrtf_header(default-48000.mhr "hrtf_default_48000")
endif()

ADD_CUSTOM_COMMAND(OUTPUT "${OpenAL_BINARY_DIR}/bsinc_inc.h"
    COMMAND "${BSINCGEN_COMMAND}" "${OpenAL_BINARY_DIR}/bsinc_inc.h"
    DEPENDS native-tools "${NATIVE_SRC_DIR}bsincgen.c"
    VERBATIM
)
SET(ALC_OBJS  ${ALC_OBJS} "${OpenAL_BINARY_DIR}/bsinc_inc.h")


IF(ALSOFT_UTILS AND NOT ALSOFT_NO_CONFIG_UTIL)
    add_subdirectory(utils/alsoft-config)
ENDIF()
IF(ALSOFT_EXAMPLES)
    IF(NOT SDL2_FOUND)
        FIND_PACKAGE(SDL2)
    ENDIF()
    IF(SDL2_FOUND)
        FIND_PACKAGE(SDL_sound)
        FIND_PACKAGE(FFmpeg COMPONENTS AVFORMAT AVCODEC AVUTIL SWSCALE SWRESAMPLE)
    ENDIF()
ENDIF()

IF(NOT WIN32)
    SET(LIBNAME "openal")
ELSE()
    SET(LIBNAME "OpenAL32")
ENDIF()

# Needed for openal.pc.in
SET(prefix ${CMAKE_INSTALL_PREFIX})
SET(exec_prefix "\${prefix}")
SET(libdir "\${exec_prefix}/${CMAKE_INSTALL_LIBDIR}")
SET(bindir "\${exec_prefix}/${CMAKE_INSTALL_BINDIR}")
SET(includedir "\${prefix}/${CMAKE_INSTALL_INCLUDEDIR}")
SET(PACKAGE_VERSION "${LIB_VERSION}")
SET(PKG_CONFIG_CFLAGS )
SET(PKG_CONFIG_PRIVATE_LIBS )
IF(LIBTYPE STREQUAL "STATIC")
    SET(PKG_CONFIG_CFLAGS -DAL_LIBTYPE_STATIC)
    FOREACH(FLAG  ${LINKER_FLAGS} ${EXTRA_LIBS} ${MATH_LIB})
        # If this is already a linker flag, or is a full path+file, add it
        # as-is. Otherwise, it's a name intended to be dressed as -lname.
        IF(FLAG MATCHES "^-.*" OR EXISTS "${FLAG}")
            SET(PKG_CONFIG_PRIVATE_LIBS "${PKG_CONFIG_PRIVATE_LIBS} ${FLAG}")
        ELSE()
            SET(PKG_CONFIG_PRIVATE_LIBS "${PKG_CONFIG_PRIVATE_LIBS} -l${FLAG}")
        ENDIF()
    ENDFOREACH()
ENDIF()

# End configuration
CONFIGURE_FILE(
    "${OpenAL_SOURCE_DIR}/config.h.in"
    "${OpenAL_BINARY_DIR}/config.h")
CONFIGURE_FILE(
    "${OpenAL_SOURCE_DIR}/openal.pc.in"
    "${OpenAL_BINARY_DIR}/openal.pc"
    @ONLY)


# Add a static library with common functions used by multiple targets
ADD_LIBRARY(common STATIC ${COMMON_OBJS})
TARGET_COMPILE_DEFINITIONS(common PRIVATE ${CPP_DEFS})
TARGET_COMPILE_OPTIONS(common PRIVATE ${C_FLAGS})


UNSET(HAS_ROUTER)
SET(IMPL_TARGET OpenAL)
SET(COMMON_LIB )
SET(SUBSYS_FLAG )

# Build main library
IF(LIBTYPE STREQUAL "STATIC")
    SET(CPP_DEFS ${CPP_DEFS} AL_LIBTYPE_STATIC)
    IF(WIN32 AND ALSOFT_NO_UID_DEFS)
        SET(CPP_DEFS ${CPP_DEFS} AL_NO_UID_DEFS)
    ENDIF()
    ADD_LIBRARY(OpenAL STATIC ${COMMON_OBJS} ${OPENAL_OBJS} ${ALC_OBJS})
ELSE()
    # Make sure to compile the common code with PIC, since it'll be linked into
    # shared libs that needs it.
    SET_PROPERTY(TARGET common PROPERTY POSITION_INDEPENDENT_CODE TRUE)
    SET(COMMON_LIB common)

    IF(WIN32)
        IF(MSVC)
            SET(SUBSYS_FLAG ${SUBSYS_FLAG} "/SUBSYSTEM:WINDOWS")
        ELSEIF(CMAKE_COMPILER_IS_GNUCC)
            SET(SUBSYS_FLAG ${SUBSYS_FLAG} "-mwindows")
        ENDIF()
    ENDIF()

    IF(WIN32 AND ALSOFT_BUILD_ROUTER)
        ADD_LIBRARY(OpenAL SHARED router/router.c router/router.h router/alc.c router/al.c)
        TARGET_COMPILE_DEFINITIONS(OpenAL
            PRIVATE AL_BUILD_LIBRARY AL_ALEXT_PROTOTYPES ${CPP_DEFS})
        TARGET_COMPILE_OPTIONS(OpenAL PRIVATE ${C_FLAGS})
        TARGET_LINK_LIBRARIES(OpenAL PRIVATE ${LINKER_FLAGS} ${COMMON_LIB})
        SET_TARGET_PROPERTIES(OpenAL PROPERTIES PREFIX "")
        SET_TARGET_PROPERTIES(OpenAL PROPERTIES OUTPUT_NAME ${LIBNAME})
        IF(TARGET build_version)
            ADD_DEPENDENCIES(OpenAL build_version)
        ENDIF()
        SET(HAS_ROUTER 1)

        SET(LIBNAME "soft_oal")
        SET(IMPL_TARGET soft_oal)
    ENDIF()

    ADD_LIBRARY(${IMPL_TARGET} SHARED ${OPENAL_OBJS} ${ALC_OBJS})
    IF(WIN32)
        SET_TARGET_PROPERTIES(${IMPL_TARGET} PROPERTIES PREFIX "")
    ENDIF()
ENDIF()
SET_TARGET_PROPERTIES(${IMPL_TARGET} PROPERTIES OUTPUT_NAME ${LIBNAME}
    VERSION ${LIB_VERSION}
    SOVERSION ${LIB_MAJOR_VERSION}
)
TARGET_COMPILE_DEFINITIONS(${IMPL_TARGET}
    PRIVATE AL_BUILD_LIBRARY AL_ALEXT_PROTOTYPES ${CPP_DEFS})
TARGET_INCLUDE_DIRECTORIES(${IMPL_TARGET}
    PRIVATE "${OpenAL_SOURCE_DIR}/OpenAL32/Include" "${OpenAL_SOURCE_DIR}/Alc" ${INC_PATHS})
TARGET_COMPILE_OPTIONS(${IMPL_TARGET} PRIVATE ${C_FLAGS})
TARGET_LINK_LIBRARIES(${IMPL_TARGET}
    PRIVATE ${LINKER_FLAGS} ${COMMON_LIB} ${EXTRA_LIBS} ${MATH_LIB})
IF(TARGET build_version)
    ADD_DEPENDENCIES(${IMPL_TARGET} build_version)
ENDIF()

IF(WIN32 AND MINGW AND ALSOFT_BUILD_IMPORT_LIB AND NOT LIBTYPE STREQUAL "STATIC")
    FIND_PROGRAM(SED_EXECUTABLE NAMES sed DOC "sed executable")
    FIND_PROGRAM(DLLTOOL_EXECUTABLE NAMES "${DLLTOOL}" DOC "dlltool executable")
    IF(NOT SED_EXECUTABLE OR NOT DLLTOOL_EXECUTABLE)
        MESSAGE(STATUS "")
        IF(NOT SED_EXECUTABLE)
            MESSAGE(STATUS "WARNING: Cannot find sed, disabling .def/.lib generation")
        ENDIF()
        IF(NOT DLLTOOL_EXECUTABLE)
            MESSAGE(STATUS "WARNING: Cannot find dlltool, disabling .def/.lib generation")
        ENDIF()
    ELSE()
        SET_PROPERTY(TARGET OpenAL APPEND_STRING PROPERTY LINK_FLAGS
                        " -Wl,--output-def,OpenAL32.def")
        ADD_CUSTOM_COMMAND(TARGET OpenAL POST_BUILD
            COMMAND "${SED_EXECUTABLE}" -i -e "s/ @[^ ]*//" OpenAL32.def
            COMMAND "${DLLTOOL_EXECUTABLE}" -d OpenAL32.def -l OpenAL32.lib -D OpenAL32.dll
            COMMENT "Stripping ordinals from OpenAL32.def and generating OpenAL32.lib..."
            VERBATIM
        )
    ENDIF()
ENDIF()

set(OpenAL_INCLUDE_DIRS ${OpenAL_SOURCE_DIR}/include PRAENT_DIRECTORY)