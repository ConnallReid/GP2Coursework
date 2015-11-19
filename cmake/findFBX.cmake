# Locate FBX
# This module defines:
# FBX_INCLUDE_DIR, where to find the headers
#
# FBX_LIBRARY, FBX_LIBRARY_DEBUG
# FBX_FOUND
#
# $FBX_DIR is an environment variable that would
# correspond to the ./configure --prefix=$FBX_DIR
#
# $LIB_VER is a variable defined for type -md or -mt, 
# if not assumed plain lib

IF(APPLE)
	SET(FBX_LIBDIR "clang")
	SET(FBX_LIBEXT "a")
ELSEIF(CMAKE_COMPILER_IS_GNUCXX)
  SET(FBX_LIBDIR "gcc4")
ELSEIF(MSVC80)
  SET(FBX_LIBDIR "vs2005")
ELSEIF(MSVC90)
  SET(FBX_LIBDIR "vs2008")
ELSEIF(MSVC10)
  SET(FBX_LIBDIR "vs2010")
ELSEIF(MSVC11)
  SET(FBX_LIBDIR "vs2012")
ELSEIF(MSVC12 OR MSVC_VERSION>1800)
  SET(FBX_LIBDIR "vs2013")
  SET(FBX_LIBEXT "lib")
ENDIF()

IF(APPLE)
  	    # do nothing
ELSEIF(CMAKE_CL_64)
  SET(FBX_LIBDIR ${FBX_LIBDIR}/x64)
ELSEIF(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_SIZEOF_VOID_P EQUAL 8)
  SET(FBX_LIBDIR ${FBX_LIBDIR}/x64)
ELSE()
  SET(FBX_LIBDIR ${FBX_LIBDIR}/x86)
ENDIF()

SET(FBX_LIBNAME "libfbxsdk")

SET(FBX_LIBNAME_DEBUG ${FBX_LIBNAME})

SET( FBX_SEARCH_PATHS
	    $ENV{FBX_DIR}
	    "$ENV{ProgramW6432}/Autodesk/FBX/FBX SDK/2015.1"
	    "$ENV{PROGRAMFILES}/Autodesk/FBX/FBX SDK/2015.1"
	    /Applications/Autodesk/FBXSDK20151
      "$ENV{ProgramW6432}/Autodesk/FBX/FBX SDK/2014.2"
      "$ENV{PROGRAMFILES}/Autodesk/FBX/FBX SDK/2014.2"
      /Applications/Autodesk/FBXSDK20142
      /Applications/Autodesk/FBXSDK20141
      "/Applications/Autodesk/FBX SDK/2016.0")

#I think the last line in the search path is an old typo, but let's search for 2014.1 anyway - LV
# search for headers & debug/release libraries
FIND_PATH(FBX_INCLUDE_DIR "fbxsdk.h"
  PATHS ${FBX_SEARCH_PATHS}
  PATH_SUFFIXES "include")

  FIND_LIBRARY( FBX_LIBRARY "${FBX_LIBNAME}.${FBX_LIBEXT}"
    PATHS ${FBX_SEARCH_PATHS}
  PATH_SUFFIXES "lib/${FBX_LIBDIR}/release")

#Once one of the calls succeeds the result variable will be set and stored in the cache so that no call will search again.

#no debug d suffix, search in debug folder only
FIND_LIBRARY( FBX_LIBRARY_DEBUG "${FBX_LIBNAME}.${FBX_LIBEXT}"
  PATHS ${FBX_SEARCH_PATHS}
PATH_SUFFIXES "lib/${FBX_LIBDIR}/debug")

IF(FBX_LIBRARY AND FBX_LIBRARY_DEBUG AND FBX_INCLUDE_DIR)
  SET(FBX_FOUND "YES")
ELSE()
  SET(FBX_FOUND "NO")
ENDIF()
