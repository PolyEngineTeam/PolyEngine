cmake_minimum_required(VERSION 3.7)
project(PolyEngine VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if($ENV{TRAVIS})
	if($ENV{TRAVIS} STREQUAL "true")
		set(IS_TRAVIS_BUILD TRUE)
	endif($ENV{TRAVIS} STREQUAL "true")
endif($ENV{TRAVIS})

#Options and whatnot
option(SIMD "Enable SSE3 extensions" ON)
set(SANITIZER none CACHE STRING "C++ Sanitizer. Possible values: none, address, undefined, memory, thread") #TODO(vuko)
set(MEMSAN_LIBCXX "" CACHE STRING "Instrumented C++ standard library location for MemorySanitizer") #TODO(vuko)
option(CLANG_TIDY "Enable clang-tidy static analyzer" OFF) #TODO(vuko): clang-tidy -checks='cppcoreguidelines-*,clang-analyzer-*,-clang-analyzer-alpha*,-clang-analyzer-llvm.Conventions,-clang-analyzer-optin*,-clang-analyzer-osx,misc-*,modernize-*,performance-*,readability-*,google-explicit-constructor,google-readability-casting,google-runtime-int,google-runtime-memset,google-runtime-operator,llvm-namespace-comment' (maybe minus cppcoreguidelines-pro-type-union-access?)
set(CLANG_TIDY_EXE "" CACHE STRING "clang-tidy executable location") #TODO(vuko)
option(BUILD_TESTS "Determines whether to build tests" ON)
option(GENERATE_COVERAGE "Enable line coverage info generated from unit test runs" OFF)
option(GENERATE_DOXYGEN "Generate API documentation if Doxygen is available" ON)

if(GENERATE_COVERAGE)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU") #TODO(vuko): clang and MSVC
		# this is an undocumented option for changing the output extension from *.cpp.ext
		# to *.ext; As there is no reasonable way to obtain the list of object files
		# this seems to be the best option to generate the naming gcov expects.
		set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)
		set(GCOV_COMMAND gcov CACHE STRING "Command to invoke to run gcov.")
	endif()
endif()

#target_compile_features(<target name> PUBLIC cxx_static_assert) #TODO(vuko): will this be useful?

#Build switches
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang") #TODO(vuko): cleanup, build types (debug, etc)
	add_compile_options(-pipe)
	add_compile_options(-Wall)
	add_compile_options(-Wextra)
	#add_compile_options(-stdlib=libc++)
	#add_compile_options(-lc++abi)
	if(SIMD)
		add_compile_options(-msse3)
	endif(SIMD)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	add_compile_options(-pipe)
	add_compile_options(-Wall)
	add_compile_options(-Wextra)
	if(SIMD)
		add_compile_options(-msse3)
	endif(SIMD)
else()
	message(FATAL_ERROR "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}") #TODO(vuko): MSVC options
endif()

#Is our Windows 32 or 64-bit?
if(WIN32)
	if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
		set(WIN32_X64 1)
		set(WIN32_X86 0)
	else("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
		set(WIN32_X64 0)
		set(WIN32_X86 1)
	endif("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
else(WIN32)
	set(WIN32_X64 0)
	set(WIN32_X86 0)
endif(WIN32)

#Finally, meat and potatoes
add_subdirectory(Core)
add_subdirectory(Engine)
add_subdirectory(Game)
if(WIN32)
	add_subdirectory(WindowsGLStandalone)
else(WIN32)
	add_subdirectory(Standalone/Linux)
endif(WIN32)
if(BUILD_TESTS)
	enable_testing()
	add_subdirectory(UnitTests)
endif(BUILD_TESTS)

#Doxygen
if(GENERATE_DOXYGEN)
	find_package(Doxygen)
	if(DOXYGEN_FOUND AND false) #TODO(vuko): need to write a Doxyfile.in first, then reenable
		set(DOC_DIR ${PROJECT_BINARY_DIR}/doc)
		configure_file(${PROJECT_SOURCE_DIR}/Docs/Doxyfile.in ${GB_LIB_DOC_DIR}/Doxyfile @ONLY)
		add_custom_target(doxygen-doc
			COMMAND ${DOXYGEN_EXECUTABLE}
			WORKING_DIRECTORY ${DOC_DIR}
			COMMENT "Building doxygen documentation"
			SOURCES ${PROJECT_SOURCE_DIR}/Docs/Doxyfile.in
		)
		source_group("" FILES ${PROJECT_SOURCE_DIR}/Docs/Doxyfile.in)
	endif()
endif()

#TODO(vuko): install and stuff
