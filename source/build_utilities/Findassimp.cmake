if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(ASSIMP_ARCHITECTURE "/x64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
	set(ASSIMP_ARCHITECTURE "/x86")
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)
	
if(WIN32)
	set(ASSIMP_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../extern/assimp")
	
	# Find path of each library
	find_path(assimp_INCLUDE_DIRS
		NAMES
			assimp/anim.h
		HINTS
			${ASSIMP_ROOT_DIR}/include
	)
	
	message("${assimp_INCLUDE_DIRS}")
	
	if(MSVC12)
		set(ASSIMP_MSVC_VERSION "vc120")
	elseif(MSVC14)	
		set(ASSIMP_MSVC_VERSION "vc142")
	endif(MSVC12)
	
	if(MSVC12 OR MSVC14)
	
		find_path(ASSIMP_LIBRARY_DIR
			NAMES
				assimp-${ASSIMP_MSVC_VERSION}-mtd.lib
			HINTS
				${ASSIMP_ROOT_DIR}/lib${ASSIMP_ARCHITECTURE}
		)
		
		find_library(ASSIMP_LIBRARY_RELEASE				assimp-${ASSIMP_MSVC_VERSION}-mtd.lib 			PATHS ${ASSIMP_LIBRARY_DIR})
		find_library(ASSIMP_LIBRARY_DEBUG				assimp-${ASSIMP_MSVC_VERSION}-mtd.lib			PATHS ${ASSIMP_LIBRARY_DIR})
		
		set(ASSIMP_LIBRARY 
			optimized 	${ASSIMP_LIBRARY_RELEASE}
			debug		${ASSIMP_LIBRARY_DEBUG}
		)
		
		message(STATUS "evaluated to ${ASSIMP_LIBRARY_DEBUG}, ${ASSIMP_LIBRARY_RELEASE}")
		set(assimp_LIBRARIES ${ASSIMP_LIBRARY_DEBUG})
		set(assimp_DLLS "${ASSIMP_ROOT_DIR}/bin${ASSIMP_ARCHITECTURE}/assimp-${ASSIMP_MSVC_VERSION}-mtd.dll")
	
	endif()
	
else(WIN32)

	find_path(
	  assimp_INCLUDE_DIRS
	  NAMES assimp/postprocess.h assimp/scene.h assimp/version.h assimp/config.h assimp/cimport.h
	  PATHS /usr/local/include
	  PATHS /usr/include/

	)

	find_library(
	  assimp_LIBRARIES
	  NAMES assimp
	  PATHS /usr/local/lib/
	  PATHS /usr/lib64/
	  PATHS /usr/lib/
	)

	if (assimp_INCLUDE_DIRS AND assimp_LIBRARIES)
	  SET(assimp_FOUND TRUE)
	ENDIF (assimp_INCLUDE_DIRS AND assimp_LIBRARIES)

	if (assimp_FOUND)
	  if (NOT assimp_FIND_QUIETLY)
		message(STATUS "Found asset importer library: ${assimp_LIBRARIES}")
	  endif (NOT assimp_FIND_QUIETLY)
	else (assimp_FOUND)
	  if (assimp_FIND_REQUIRED)
		message(FATAL_ERROR "Could not find asset importer library")
	  endif (assimp_FIND_REQUIRED)
	endif (assimp_FOUND)
	
endif(WIN32)