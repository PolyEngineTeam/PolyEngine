cmake_minimum_required(VERSION 3.6)
cmake_policy(VERSION 3.7)
project(PolyEngine VERSION 0.0.1 LANGUAGES CXX)

#Initial setup: C++ standard version, build types, CMake modules
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) #for portability
set(CMAKE_CONFIGURATION_TYPES "Debug;DebugFast;Testing;Release" CACHE STRING "" FORCE)
set_property(GLOBAL PROPERTY DEBUG_CONFIGURATIONS "Debug;DebugFast") #Tell CMake which configurations are debug
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "DebugFast" "Testing" "Release") #CMake GUIs may use this to provide a selection widget instead of generic text field
mark_as_advanced(CMAKE_CONFIGURATION_TYPES)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules")

#Are we running Travis?
if($ENV{TRAVIS})
	if($ENV{TRAVIS} STREQUAL "true")
		set(IS_TRAVIS_BUILD TRUE)
	endif($ENV{TRAVIS} STREQUAL "true")
endif($ENV{TRAVIS})

#32 or 64-bit?
if(CMAKE_SIZEOF_VOID_P EQUAL "8")
	set(ARCH_X64 1)
	set(ARCH_X86 0)
else(CMAKE_SIZEOF_VOID_P EQUAL "8")
	set(ARCH_X64 0)
	set(ARCH_X86 1)
endif(CMAKE_SIZEOF_VOID_P EQUAL "8")

#Options and whatnot
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type. One of: Debug, DebugFast (debug with some optimizations), Testing (heavy optimizations, no debug info), Release (same as Testing, but with all assertions off)")
option(SIMD "Enable SSE3 extensions for faster code (requires a compatible CPU)" ON)
option(CLANG_TIDY_ON_BUILD "Run clang-tidy static analyzer while compiling" OFF)
find_program(CLANG_TIDY_EXE NAMES clang-tidy DOC "Path to clang-tidy executable")
find_program(RUN_CLANG_TIDY_PY NAMES run-clang-tidy.py PATHS "/usr/share/clang" DOC "Path to run-clang-tidy.py script")
option(BUILD_TESTS "Determines whether to build tests" ON)
option(GENERATE_COVERAGE "Enable line coverage info generated from unit test runs (GCC only)" OFF)
option(GENERATE_DOXYGEN "Generate API documentation if Doxygen is available" ON)
find_program(GCOV_COMMAND NAMES gcov "Path to gcov executable")
option(USE_BUNDLED_CATCH "Do not try to fetch Catch and use the bundled copy instead" OFF)

string(STRIP "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE)
if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
	message(STATUS "Setting build type to Debug as none was specified")
	set(CMAKE_BUILD_TYPE Debug)
elseif(NOT CMAKE_BUILD_TYPE MATCHES "(^Debug$|^DebugFast$|^Testing$|^Release$)")
	message(WARNING "Invalid build type \"${CMAKE_BUILD_TYPE}\" specified!\nValid values are: Debug, DebugFast, Testing, Release.\nDefaulting to Debug.")
	set(CMAKE_BUILD_TYPE Debug)
endif()

if(CLANG_TIDY_ON_BUILD AND CLANG_TIDY_EXE)
	set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
endif()

if(GENERATE_COVERAGE)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU") #MAYBE(vuko): clang
		# this is an undocumented option for changing the output extension from *.cpp.ext
		# to *.ext; As there is no reasonable way to obtain the list of object files
		# this seems to be the best option to generate the naming gcov expects.
		set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)
	else()
		message(WARNING "Coverage with compilers other than GCC is not supported yet.")
	endif()
endif()

#Build switches
if(CMAKE_CXX_COMPILER_ID MATCHES "(Clang|^GNU$)")
	add_compile_options(-pipe)
	add_compile_options(-Wall)
	add_compile_options(-Wextra)
	add_compile_options(-fvisibility=hidden)
	add_compile_options($<$<BOOL:${SIMD}>:-msse3>)
	add_compile_options($<$<STREQUAL:${CMAKE_GENERATOR},Ninja>:-fdiagnostics-color=always>)
	add_compile_options($<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugFast>>:-g>)
	add_compile_options($<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugFast>>:-fno-omit-frame-pointer>)
	add_compile_options($<$<CONFIG:DebugFast>:-O1>)
	add_compile_options($<$<OR:$<CONFIG:Testing>,$<CONFIG:Release>>:-O3>)
	#TODO(vuko): build type macro definitions (once we decide on them)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_compile_options(/Wall)
	add_compile_options($<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugFast>>:/Zi>) #debug symbols
	add_compile_options($<$<CONFIG:DebugFast>:/O1>)
	add_compile_options($<$<OR:$<CONFIG:Testing>,$<CONFIG:Release>>:/Ox>) #max optimizations
else()
	message(FATAL_ERROR "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

#Clang Sanitizers support
include(sanitizers)

#Enable CCache if found
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

#Precompiled headers support (using cotire)
include(cotire)
set(COTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES "-j")
if(CCACHE_FOUND AND COMMAND cotire)
	set(ENV{CCACHE_SLOPPINESS} "pch_defines,time_macros")
endif(CCACHE_FOUND AND COMMAND cotire)

#Finally, meat and potatoes
add_subdirectory(Core)
add_subdirectory(Engine)
add_subdirectory(Game)
if(NOT SANITIZER_MEMORY)
	if(WIN32)
		add_subdirectory(WindowsGLStandalone)
	else(WIN32)
		add_subdirectory(Standalone/Linux)
	endif(WIN32)
else()
	message(STATUS "MemorySanitizer detected. Standalone disabled due to many shared library dependencies.")
	message(STATUS "MemorySanitizer requires for EVERY dependency to be also instrumented.")
	message(STATUS "Hint: consider using Valgrind instead.")
endif()
if(BUILD_TESTS)
	enable_testing()
	add_subdirectory(UnitTests)
endif(BUILD_TESTS)

#Doxygen
if(GENERATE_DOXYGEN)
	find_package(Doxygen)
	if(DOXYGEN_FOUND AND false) #TODO(vuko): reenable after Doxyfile.in gets written
		set(DOC_DIR ${PROJECT_BINARY_DIR}/doc)
		configure_file(${PROJECT_SOURCE_DIR}/Doxygen/Doxyfile.in ${DOC_DIR}/Doxyfile @ONLY)
		add_custom_target(docs
		                  COMMAND ${DOXYGEN_EXECUTABLE}
		                  WORKING_DIRECTORY ${DOC_DIR}
		                  COMMENT "Building doxygen documentation"
		                  SOURCES ${PROJECT_SOURCE_DIR}/Doxygen/Doxyfile.in
		)
		source_group("" FILES ${PROJECT_SOURCE_DIR}/Docs/Doxyfile.in)
	endif()
endif()

#clang-tidy custom target for when you want to run it independent of the compilation
add_custom_command(OUTPUT "compile_commands.json" generate_compilation_db COMMAND "${CMAKE_COMMAND}" "${CMAKE_BINARY_DIR}" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON)
if(RUN_CLANG_TIDY_PY)   #we got the script, use it
	add_custom_target(clang-tidy COMMAND echo ${RUN_CLANG_TIDY_PY} -p "${CMAKE_BINARY_DIR}" DEPENDS "compile_commands.json")
else(CMAKE_VERSION VERSION_EQUAL 3.8 OR CMAKE_VERSION VERSION_GREATER 3.8) #don't have it, replicate it somewhat (needs CMake 3.7 for BUILDSYSTEM_TARGETS property and 3.8 for COMMAND_EXPAND_LISTS; ironically VERSION_GREATER_EQUAL also needs 3.7)
	set(CLANG_TIDY_SOURCES "")
	get_directory_property(subdirs SUBDIRECTORIES)
	foreach(dir IN LISTS subdirs)
		get_directory_property(targets DIRECTORY ${dir} BUILDSYSTEM_TARGETS)
		foreach(target IN LISTS targets)
			get_target_property(type ${target} TYPE)
			if(type MATCHES "(EXECUTABLE|(SHARED|STATIC)_LIBRARY)")
				get_target_property(sources ${target} SOURCES)
				list(FILTER sources INCLUDE REGEX ".cpp$")
				list(APPEND CLANG_TIDY_SOURCES "$<TARGET_PROPERTY:${target},SOURCE_DIR>/$<JOIN:${sources},;$<TARGET_PROPERTY:${target},SOURCE_DIR>/>")
				#list(APPEND CLANG_TIDY_SOURCES "$<TARGET_PROPERTY:${target},SOURCE_DIR>/$<JOIN:$<TARGET_PROPERTY:${target},SOURCES>,;$<TARGET_PROPERTY:${target},SOURCE_DIR>/>")
			endif()
		endforeach()
	endforeach()
	add_custom_target(clang-tidy COMMAND echo ${CLANG_TIDY_EXE} -p "${CMAKE_BINARY_DIR}" "${CLANG_TIDY_SOURCES}" DEPENDS "compile_commands.json" WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}" COMMAND_EXPAND_LISTS)
endif()

#TODO(vuko): install and stuff
