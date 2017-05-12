#Taken from the official assimp repository (rev 270355f). Modified slightly.
#License file can be found in ThirdParty/assimp.

#NOTE(vuko): older versions of assimp unfortunately do not provide an assimp-config.cmake file
#            which is why we need this

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(ASSIMP_ARCHITECTURE "x64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
	set(ASSIMP_ARCHITECTURE "Win32")
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

if(WIN32)
	set(ASSIMP_ROOT_DIR CACHE PATH "ASSIMP root directory")

	# Find path of each library
	find_path(ASSIMP_INCLUDE_DIRS
		NAMES
			assimp/anim.h
		HINTS
			${ASSIMP_ROOT_DIR}/include
	)

	if(MSVC12)
		set(ASSIMP_MSVC_VERSION "vc120")
	elseif(MSVC14)
		set(ASSIMP_MSVC_VERSION "vc140")
	endif(MSVC12)

	if(MSVC12 OR MSVC14)

		find_path(ASSIMP_LIBRARY_DIR
			NAMES
				assimp-${ASSIMP_MSVC_VERSION}-mt.lib
			HINTS
				${ASSIMP_ROOT_DIR}/lib/Release/${ASSIMP_ARCHITECTURE}
		)

		find_library(ASSIMP_LIBRARY_RELEASE				assimp-${ASSIMP_MSVC_VERSION}-mt.lib 			PATHS ${ASSIMP_LIBRARY_DIR})
		find_library(ASSIMP_LIBRARY_DEBUG				assimp-${ASSIMP_MSVC_VERSION}-mtd.lib			PATHS ${ASSIMP_LIBRARY_DIR})

		set(ASSIMP_LIBRARIES "${ASSIMP_LIBRARY_RELEASE}")

		FUNCTION(ASSIMP_COPY_BINARIES TargetDirectory)
			ADD_CUSTOM_TARGET(AssimpCopyBinaries
				COMMAND ${CMAKE_COMMAND} -E copy ${ASSIMP_ROOT_DIR}/bin${ASSIMP_ARCHITECTURE}/assimp-${ASSIMP_MSVC_VERSION}-mtd.dll 	${TargetDirectory}/Debug/assimp-${ASSIMP_MSVC_VERSION}-mtd.dll
				COMMAND ${CMAKE_COMMAND} -E copy ${ASSIMP_ROOT_DIR}/bin${ASSIMP_ARCHITECTURE}/assimp-${ASSIMP_MSVC_VERSION}-mt.dll 		${TargetDirectory}/Release/assimp-${ASSIMP_MSVC_VERSION}-mt.dll
			COMMENT "Copying Assimp binaries to '${TargetDirectory}'"
			VERBATIM)
		ENDFUNCTION(ASSIMP_COPY_BINARIES)

	endif()

else(WIN32)

	find_path(
	  ASSIMP_INCLUDE_DIRS
	  NAMES postprocess.h scene.h version.h config.h cimport.h
	  PATHS /usr/local/include/
	  PATH_SUFFIXES assimp
	)

	find_library(
	  ASSIMP_LIBRARIES
	  NAMES assimp assimp3.0
	  PATHS /usr/local/include/
	)

endif(WIN32)

if (NOT ASSIMP_INCLUDE_DIRS AND ASSIMP_LIBRARIES)
	message(FATAL_ERROR "Found Assimp libraries, but not includes. Installed version is most likely too old!")
endif()

if (ASSIMP_INCLUDE_DIRS AND ASSIMP_LIBRARIES)
	SET(ASSIMP_FOUND TRUE)
ENDIF (ASSIMP_INCLUDE_DIRS AND ASSIMP_LIBRARIES)

if (ASSIMP_FOUND)
	if (NOT assimp_FIND_QUIETLY)
		message(STATUS "Found asset importer library: ${ASSIMP_LIBRARIES}")
	endif (NOT assimp_FIND_QUIETLY)
else (ASSIMP_FOUND)
	if (assimp_FIND_REQUIRED)
		message(FATAL_ERROR "Could not find asset importer library")
	endif (assimp_FIND_REQUIRED)
endif (ASSIMP_FOUND)
