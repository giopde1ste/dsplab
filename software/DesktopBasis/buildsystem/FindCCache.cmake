## CMake makefile
## Copyright 2001-2014 Kiwanda Embedded Systemen
## $Id: FindCCache.cmake 4389 2022-02-24 14:42:24Z ewout $

# Locate CCache in a system


find_program(CCACHE
NAMES ccache)

# handle REQUIRED and QUIET options
include(FindPackageHandleStandardArgs)
if (CMAKE_VERSION LESS 2.8.3)
find_package_handle_standard_args(CCache DEFAULT_MSG CCACHE)
else ()
find_package_handle_standard_args(CCache REQUIRED_VARS CCACHE)
endif ()

if(CCACHE)
# zoek de bijbehorende lib dir waarin compiler synmlinks zijn geplaatst
find_path(CCACHE_LIBDIRBASIS ccache/cc
  PATH_PREFIX ccache
  PATHS 
  /usr/local/libexec
  /opt/local/libexec
  /usr/lib )
endif(CCACHE)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
SET(CCACHE_FLAGS "-Qunused-arguments -fcolor-diagnostics")
#set env CCACHE_CPP2=yes
SET(ENV{CCACHE_CPP} YES)
endif()

if(CCACHE AND CCACHE_LIBDIRBASIS)
 set(CCACHE_LIBDIR ${CCACHE_LIBDIRBASIS}/ccache)
  message(STATUS "Found CCache Lib dir : ${CCACHE_LIBDIR}")
endif(CCACHE AND CCACHE_LIBDIRBASIS)

mark_as_advanced (
CCACHE
CCACHE_FLAGS
CCACHE_LIBDIR
)

