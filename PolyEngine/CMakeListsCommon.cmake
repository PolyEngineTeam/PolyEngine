cmake_minimum_required(VERSION 3.2)

# Allow use of folders for projects in some IDEs (VS, Xcode, etc...)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Options
OPTION( BUILD_SHARED_LIBS
  "Build package with shared libraries."
  ON
)

OPTION( BUILD_FRAMEWORK
  "Build package as Mac OS X Framework bundle."
  OFF
)

if (UNIX AND NOT APPLE)
   set(LINUX TRUE)
else()
   set(LINUX FALSE)
endif()

# Target names
set(CORE_TARGET PolyCore)
set(ENGINE_TARGET PolyEngine)
set(GLDEVICE_TARGET PolyRenderingDeviceGL)
set(EDITOR_TARGET PolyEditor)
set(UNITTESTS_TARGET PolyUnitTests)
set(STANDALONE_TARGET PolyStandalone)
set(ENGINEAPI_TARGET PolyEngineAPI)

# Prepend our modules dir to the path
list(INSERT CMAKE_MODULE_PATH 0 "${ENGINE_ROOT_DIR}/CMakeModules")

##C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) #for portability

##
# Are we running CI?
##
set(CI_BUILD FALSE)

if($ENV{TRAVIS})
	string( TOLOWER "$ENV{TRAVIS}" TRAVIS_LOWER )
	if(${TRAVIS_LOWER} STREQUAL "true")
		set(CI_BUILD TRUE)
	endif()
endif()

if($ENV{APPVEYOR})
	string( TOLOWER "$ENV{APPVEYOR}" APPVEYOR_LOWER )
	if(${APPVEYOR_LOWER} STREQUAL "true")
		set(CI_BUILD TRUE)
	endif()
endif()
message("CI Build: ${CI_BUILD}")

##
# 32 or 64-bit?
##
if(CMAKE_SIZEOF_VOID_P EQUAL "8")
	set(ARCH_X64 1)
	set(ARCH_X86 0)
else(CMAKE_SIZEOF_VOID_P EQUAL "8")
	set(ARCH_X64 0)
	set(ARCH_X86 1)
endif(CMAKE_SIZEOF_VOID_P EQUAL "8")

# Static libraries need to have all symbols exported in the shared library on *NIX systems.
if(LINUX)
	set(BEGIN_EXPORT_ALL_SYMBOLS -Wl,-whole-archive)
	set(END_EXPORT_ALL_SYMBOLS -Wl,-no-whole-archive)
elseif(APPLE)
	set(BEGIN_EXPORT_ALL_SYMBOLS -Wl,-all_load)
	set(END_EXPORT_ALL_SYMBOLS -Wl,-noall_load)
else()
	set(BEGIN_EXPORT_ALL_SYMBOLS)
	set(END_EXPORT_ALL_SYMBOLS)
endif()

# Make sure qt is defined
if(DEFINED ENV{QTDIR})
	list(APPEND CMAKE_PREFIX_PATH $ENV{QTDIR})
elseif(APPLE)
	list(APPEND CMAKE_PREFIX_PATH /usr/local/opt/qt)
endif()

find_package(Qt5 COMPONENTS Core Widgets)
if(NOT DEFINED Qt5_FOUND)
	message(FATAL_ERROR "Qt5 not found. When using a standalone Qt5 toolchain, please setup QTDIR environment variable to point to it.")
endif()

if (WIN32)
	set(BULLET_ROOT "${ENGINE_ROOT_DIR}/ThirdParty/Bullet")
endif()


##
# Options and whatnot
##

##build type, SIMD
#set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
#    "Build type. One of: Debug, DebugFast (debug with some optimizations), Testing (heavy optimizations, no debug info), Release (same as Testing, but with all assertions off)"
#)
if(ARCH_X64)
	option(SIMD "Enable SSE4.2 extensions for faster code (requires a compatible CPU)" ON)
endif()

##tests, coverage, documentation
option(GENERATE_DOXYGEN "Generate API documentation if Doxygen is available" ON)

##Build types
set(CMAKE_CONFIGURATION_TYPES "Debug;DebugFast;Testing;Release" CACHE STRING "" FORCE)
#Tell CMake which configurations are debug
set_property(GLOBAL PROPERTY DEBUG_CONFIGURATIONS "Debug;DebugFast")
#CMAKE_BUILD_TYPE is not applicable to VS generators
if(NOT CMAKE_GENERATOR MATCHES "Visual Studio")
	#CMake GUIs may use this to provide a selection widget instead of generic text field
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "DebugFast" "Testing" "Release")
	mark_as_advanced(CMAKE_CONFIGURATION_TYPES)
endif()

##normalize build type settings
string(STRIP "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE)
if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
	message(STATUS "Setting build type to Debug as none was specified")
	set(CMAKE_BUILD_TYPE "Debug")
elseif(NOT CMAKE_BUILD_TYPE MATCHES "(^Debug$|^DebugFast$|^Testing$|^Release$)")
	message(WARNING "Invalid build type \"${CMAKE_BUILD_TYPE}\" specified!\nValid values are: Debug, DebugFast, Testing, Release.\nDefaulting to Debug.")
	set(CMAKE_BUILD_TYPE "Debug")
endif()

if (DEFINED CMAKE_CONFIGURATION_TYPES AND NOT ${CMAKE_CFG_INTDIR} STREQUAL ".")
  # multi-config handling
  set (BUILD_CONFIG_NAME ${CMAKE_CFG_INTDIR})
else()
  # mono config handling
  set (BUILD_CONFIG_NAME ${CMAKE_BUILD_TYPE})
endif()
message("Build configuration name: ${BUILD_CONFIG_NAME}")

# Output dirs
if (DEFINED DIST_DIR)
	if (WIN32)
		set(FULL_DIST_DIR "${DIST_DIR}") # Config name is added autmatically on windows
	else()
		set(FULL_DIST_DIR "${DIST_DIR}/${BUILD_CONFIG_NAME}")
	endif()
	
	#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${FULL_DIST_DIR})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${FULL_DIST_DIR})
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${FULL_DIST_DIR})
else()
	message(ERROR "DIST_DIR not defined!")
endif ()

##
# Build switches, except Werror
##
if(CMAKE_CXX_COMPILER_ID MATCHES "(Clang|^GNU$)")
	if(CI_BUILD)
		add_definitions(-DCI_BUILD)
   endif()
   
   ## CXX only definitions
   if(CMAKE_CXX_COMPILER_ID MATCHES "GNU") #note(celeborth): this is because as of 09.2018 rapidjson does not provide release which does not raise this errors, and gcc would not accept this as a macro
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-class-memaccess")
   endif()
   if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-command-line-argument")
   endif()

   # Shared compile flags
	add_compile_options(-pipe)
	add_compile_options(-Wall)
	add_compile_options(-Wextra)
	add_compile_options(-Wno-unused-parameter)
   add_compile_options(-Wno-ignored-qualifiers)
	add_compile_options(-fPIC)
	add_compile_options($<$<STREQUAL:${CMAKE_GENERATOR},Ninja>:-fdiagnostics-color=always>)
	add_compile_options($<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugFast>>:-g>)
	add_compile_options($<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugFast>>:-fno-omit-frame-pointer>)
	add_compile_options($<$<CONFIG:Debug>:-O0>)
	add_compile_options($<$<CONFIG:DebugFast>:-O1>)
	add_compile_options($<$<OR:$<CONFIG:Testing>,$<CONFIG:Release>>:-O3>)
	add_compile_options($<$<CONFIG:Release>:-DFINAL>) #NOTE(vuko): add_definitions() does not support generator expressions
	add_compile_options($<$<CONFIG:Release>:-mtune=native>)
	set(SIMD_FLAGS -msse4.2 -mfpmath=sse) #note(vuko): $<CXX_COMPILER_ID:comp> and cotire do not get along, see cotire issue #124
	link_libraries($<$<PLATFORM_ID:Linux>:-Wl,--enable-new-dtags>) #use DT_RUNPATH instead of DT_RPATH
	#TODO(vuko): more build type macro definitions (once we decide on them)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	if(CI_BUILD)
		add_definitions(/DCI_BUILD)
	endif()
	add_definitions(/DNOMINMAX /D_SCL_SECURE_NO_WARNINGS /D_CRT_SECURE_NO_WARNINGS /D_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING /D_SILENCE_CXX17_UNCAUGHT_EXCEPTION_DEPRECATION_WARNING /D_ENABLE_EXTENDED_ALIGNED_STORAGE) #those macros are useless and just keep messing things up, so disable them
	add_compile_options($<$<CONFIG:Release>:/DFINAL>)

	add_compile_options($<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugFast>>:/MDd>)
	add_compile_options($<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugFast>>:/D_DEBUG>)
	add_compile_options($<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugFast>>:/D_ITERATOR_DEBUG_LEVEL=2>)

	add_compile_options($<$<OR:$<CONFIG:Testing>,$<CONFIG:Release>>:/MD>)
	add_compile_options($<$<OR:$<CONFIG:Testing>,$<CONFIG:Release>>:/D_ITERATOR_DEBUG_LEVEL=0>)
	
	add_compile_options(/EHsc)
	set(SIMD_FLAGS "") #doesn't seem we need them
	if(NOT CMAKE_GENERATOR MATCHES "Visual Studio")
		add_compile_options(/W3 /Zc:__cplusplus) #warning level 3 (includes lower levels), treat warnings as errors
		add_compile_options($<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugFast>>:/Zi>) #debug symbols
		add_compile_options($<$<CONFIG:Debug>:/Od>)
		add_compile_options($<$<CONFIG:DebugFast>:/O1>)
		add_compile_options($<$<OR:$<CONFIG:Testing>,$<CONFIG:Release>>:/Ox>) #max optimizations
	else()
		#NOTE(vuko): need to define those variables, otherwise CMake encounters internal errors
		set(CMAKE_CXX_FLAGS  "/W3 /Zc:__cplusplus")
		set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Od")
		set(CMAKE_CXX_FLAGS_DEBUGFAST "/Zi /O1")
		set(CMAKE_CXX_FLAGS_TESTING "/Ox")
		set(CMAKE_CXX_FLAGS_RELEASE "/Ox")
		set(CMAKE_SHARED_LINKER_FLAGS_DEBUGFAST "")
		set(CMAKE_SHARED_LINKER_FLAGS_TESTING "")
		set(CMAKE_EXE_LINKER_FLAGS_DEBUGFAST "")
		set(CMAKE_EXE_LINKER_FLAGS_TESTING "")
	endif()
else()
	message(FATAL_ERROR "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

##
# Function that generates source groups
##
function(GenerateSourceGoups Sources)
	foreach(Source ${Sources})
		get_filename_component(SourcePath "${Source}" PATH)
		if (WIN32)
			string(REPLACE "/" "\\" SourcePath "${SourcePath}")
		endif()
		source_group("${SourcePath}" FILES "${Source}")
	endforeach()
endfunction()

##
# Enable CCache if found
##
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
	message(STATUS "ccache will be used: ${CCACHE_FOUND}")
	set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

##
# Precompiled headers support (using cotire)
##
include(cotire)
set(COTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES "-j")
if(CCACHE_FOUND AND COMMAND cotire)
	set(ENV{CCACHE_SLOPPINESS} "pch_defines,time_macros")
endif(CCACHE_FOUND AND COMMAND cotire)

##
# Custom targets with prerequisites
##
add_custom_target(
    Prerequisites ALL
    # Make distribution directory
    COMMAND ${CMAKE_COMMAND} -E make_directory "${DIST_DIR}/${BUILD_CONFIG_NAME}"
	COMMENT "Ensuring build prerequisites are met..." VERBATIM
	DEPENDS Galogen
)

set_target_properties(Prerequisites PROPERTIES FOLDER "Engine/Misc" )

###
# Galogen generation
###
macro(GalogenGenerate LibName Api Ver)
	set(OutputDir "${CMAKE_CURRENT_BINARY_DIR}/${LibName}_${Api}_${Ver}")
	set(Filename "${OutputDir}/gl")

    file(COPY ${KHR_PLATFORM_PATH} DESTINATION "${OutputDir}/KHR/")

	add_custom_command(
		DEPENDS Galogen ${GL_XML_PATH} "${OutputDir}/KHR/khrplatform.h"
		OUTPUT "${Filename}.c" "${Filename}.h"
		COMMAND $<TARGET_FILE:Galogen> ${GL_XML_PATH} --api ${Api} --ver ${Ver} --profile core --filename ${Filename}
		COMMENT "Generating OpenGL headers API: [${Api}] VER: [${Ver}]\n" VERBATIM )

   
   add_library(${LibName} STATIC "${Filename}.c")

   # Link to GLX
   if (LINUX)
	  find_package(OpenGL REQUIRED)
	  if (OpenGL_GLX_FOUND)
		target_link_libraries(${LibName} PUBLIC OpenGL::OpenGL OpenGL::GLX)
	  else ()
	  	target_link_libraries(${LibName} PUBLIC OpenGL::GL)
	  endif()
   endif()

   if (APPLE) # Workaround for undefined NULL
   	  target_compile_definitions(${LibName} PRIVATE NULL=0)
   endif()

	target_include_directories(${LibName} PUBLIC ${OutputDir})
	set_target_properties(${LibName} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${OutputDir} IMPORTED_LOCATION $<TARGET_FILE:${LibName}>)
endmacro()

### Thirdparty
add_subdirectory(${ENGINE_ROOT_DIR}/ThirdParty ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty)

##
# Enable Werror for the rest of the build
##
if(CMAKE_CXX_COMPILER_ID MATCHES "(Clang|^GNU$)")
	add_compile_options(-Werror)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	if(NOT CMAKE_GENERATOR MATCHES "Visual Studio")
		add_compile_options(/WX) #treat warnings as errors
	else()
		#NOTE(vuko): need to define those variables, otherwise CMake encounters internal errors
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX" )
	endif()
else()
	message(FATAL_ERROR "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()