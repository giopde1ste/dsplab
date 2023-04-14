# FFTW_INCLUDE_DIR = fftw3.h
# FFTW_LIBRARIES = libfftw3.a
# FFTW_FOUND = true if FFTW3 is found

if (WIN32)

if(NOT FFTW3WindowsBasisDir)
	message(FATAL_ERROR "Voor het inlezen van de FFTW bibliotheek in Windows moet de FFTW3WindowsBasisDir zijn gezet.")
else(NOT FFTW3WindowsBasisDir)

if(PROJECT_ARCH STREQUAL "x86_64")
set(FFTW_PATH ${FFTW3WindowsBasisDir}/win64/fftw-3.3.4-dll64)
else(PROJECT_ARCH STREQUAL "x86_64")
set(FFTW_PATH ${FFTW3WindowsBasisDir}/win32/fftw-3.3.2-dll32)
endif(PROJECT_ARCH STREQUAL "x86_64")

FIND_PATH(FFTW_INCLUDE_DIR fftw3.h ${FFTW_PATH})
FIND_LIBRARY(FFTW_LIBRARIES "libfftw3-3"  ${FFTW_PATH})
endif(NOT FFTW3WindowsBasisDir)

elseif(FFTW_INCLUDE_DIRS)
  FIND_PATH(FFTW_INCLUDE_DIR fftw3.h  ${FFTW_INCLUDE_DIRS})
  FIND_LIBRARY(FFTW_LIBRARY fftw3 ${FFTW_LIBRARY_DIRS})
ELSE(FFTW_INCLUDE_DIRS)
  #  SET(TRIAL_PATHS
  #    $ENV{FFTW_HOME}/include
  #    /usr/include
  #    /usr/local/include
  #    /opt/include
  #    /usr/apps/include
  #  )
  #
  #  SET(TRIAL_LIBRARY_PATHS
  #    $ENV{FFTW_HOME}/lib
  #    /usr/lib 
  #    /usr/local/lib
  #    /opt/lib
  #    /sw/lib
  #    )
  #
  #  FIND_PATH(FFTW_INCLUDE_DIR fftw3.h ${TRIAL_PATHS})
  #  FIND_LIBRARY(FFTW_LIBRARY fftw3 ${TRIAL_LIBRARY_PATHS})
  FIND_PATH(FFTW_INCLUDE_DIR fftw3.h ${QMC_INCLUDE_PATHS})
  FIND_LIBRARY(FFTW_LIBRARIES fftw3 ${QMC_LIBRARY_PATHS}) 
  
endif(WIN32)

SET(FFTW_FOUND FALSE)
IF(FFTW_INCLUDE_DIR AND FFTW_LIBRARIES)
  MESSAGE(STATUS "FFTW_INCLUDE_DIR=${FFTW_INCLUDE_DIR}")
  MESSAGE(STATUS "FFTW_LIBRARIES=${FFTW_LIBRARIES}")
  SET(FFTW_FOUND TRUE)
  else()
  
  MESSAGE(WARNING "FFTW_INCLUDE_DIR=${FFTW_INCLUDE_DIR}")
  MESSAGE(WARNING "FFTW_LIBRARIES=${FFTW_LIBRARIES}")
ENDIF()

MARK_AS_ADVANCED(
   FFTW_INCLUDE_DIR
   FFTW_LIBRARIES
   FFTW_FOUND
)
