# - Find SDL2_ttf
# Find the native SDL2_ttf includes and libraries
#
#  SDL2_TTF_INCLUDE_DIR - where to find SDL2_ttf/SDL2_ttf.h, etc.
#  SDL2_TTF_LIBRARIES   - List of libraries when using libSDL_ttf.
#  SDL2_TTF_FOUND       - True if libSDL_ttf found.

if(SDL2_TTF_INCLUDE_DIR)
  # Already in cache, be silent
  set(SDL2_TTF_FIND_QUIETLY TRUE)
endif(SDL2_TTF_INCLUDE_DIR)

find_path(SDL2_TTF_INCLUDE_DIR SDL_ttf.h
  PATH_SUFFIXES SDL2
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw          # Fink
  /opt/local   # DarwinPorts
  /opt/csw     # Blastwave
  /opt
  ${SDL2DIR}/include
  $ENV{SDL2DIR}/include
  ${SDL2_TTF_DIR}/include
  $ENV{SDL2_TTF_DIR}/include)

find_library(SDL2_TTF_LIBRARY_DEBUG
  NAMES SDL2_ttf-d SDL2_ttf-s-d
  PATH_SUFFIXES lib64 lib
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw          # Fink
  /opt/local   # DarwinPorts
  /opt/csw     # Blastwave
  /opt
  ${SDL2_TTF_DIR}/lib/x64
  $ENV{SDL2_TTF_DIR}/lib/x64)

find_library(SDL2_TTF_LIBRARY_RELEASE
  NAMES SDL2_ttf SDL2_ttf-s
  PATH_SUFFIXES lib64 lib
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw          # Fink
  /opt/local   # DarwinPorts
  /opt/csw     # Blastwave
  /opt
  ${SDL2_TTF_DIR}/lib/x64
  $ENV{SDL2_TTF_DIR}/lib/x64)

if(SDL2_TTF_LIBRARY_DEBUG OR SDL2_TTF_LIBRARY_RELEASE)
  # Library found
  set(SDL2_TTF_FOUND TRUE)

  # If both were found, set SDL2_TTF_LIBRARY to the release version
  if(SDL2_TTF_LIBRARY_DEBUG AND SDL2_TTF_LIBRARY_RELEASE)
    set(SDL2_TTF_LIBRARY ${SDL2_TTF_LIBRARY_RELEASE})
  endif()

  if(SDL2_TTF_LIBRARY_DEBUG AND NOT SDL2_TTF_LIBRARY_RELEASE)
    set(SDL2_TTF_LIBRARY ${SDL2_TTF_LIBRARY_DEBUG})
  endif()

  if(NOT SDL2_TTF_LIBRARY_DEBUG AND SDL2_TTF_LIBRARY_RELEASE)
    set(SDL2_TTF_LIBRARY ${SDL2_TTF_LIBRARY_RELEASE})
  endif()
else()
  set(SDL2_TTF_FOUND FALSE)
endif()

# Handle the QUIETLY and REQUIRED arguments and set SNDFILE_FOUND to TRUE if
# all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2_TTF DEFAULT_MSG SDL2_TTF_LIBRARY SDL2_TTF_INCLUDE_DIR)

if(SDL2_TTF_FOUND)
  set(SDL2_TTF_LIBRARIES ${SDL2_TTF_LIBRARY})
else(SDL2_TTF_FOUND)
  set(SDL2_TTF_LIBRARIES)
endif(SDL2_TTF_FOUND)

mark_as_advanced(SDL2_TTF_INCLUDE_DIR
  SDL2_TTF_LIBRARY
  SDL2_TTF_LIBRARY_RELEASE
  SDL2_TTF_LIBRARY_DEBUG)
