#************************************************************************
# CMake script om STM32F412G-Discovery software te compileren.
# Copyright 2004-2018 Hogeschool van Arnhem en Nijmegen
# Opleiding Embedded Systems Engineering
# $Id: STM32F412GDiscovery.cmake 4639 2022-11-17 16:42:29Z ewout $
#************************************************************************

add_definitions(-DUSE_STM32412G_DISCOVERY)

set(F412GDiscoveryDir ${BSPDir}/STM32412G-Discovery)
 
include_directories(${F412GDiscoveryDir} ${F412GDiscoveryDir}/h ${FatFsDir} ${UtilitiesDir} ${ComponentsDir})

# voeg de Discovery bibliotheek toe bestaande uit de komponenten "Components" en "DiscoveryBasis"
add_library(STM32412G-Discovery $<TARGET_OBJECTS:ComponentsLib> $<TARGET_OBJECTS:STM32412G-DiscoveryBasis> )

# Segger_Sysview deze niet, werkt niet en geen nuttige toepassing in onderwijs.
set(F412GDiscoveryStdBibliotheken STM32412G-Discovery ${FatFsLib} ESEBasisLib  Segger_RTT AlgemeenLib STM32F4xx_HAL_Driver CMSISFIRMWARE)

if(STM32F412GDiscoveryMetGUI)
	message(STATUS "Bouw target voor STM32F412G-Discovery met LCD Scherm+Segger EmWin GUI support.")

	add_subdirectory(${UtilitiesDir} ${CMAKE_CURRENT_BINARY_DIR}/Utilities)
	
	include(${StemWinBasisDir}/Stemwin.cmake)
	include(${StemWinBasisDir}/BSP/STM32F412G-Discovery/STM32F412GDiscoveryEmWin.cmake)
	add_subdirectory(${StemWinBasisDir} ${CMAKE_CURRENT_BINARY_DIR}/STemWin)

	set(F412GDiscoveryBibliotheken  ${F412GDiscoveryStdBibliotheken} STemWinLib STemWinOSLib UtilitiesLib )

else(STM32F412GDiscoveryMetGUI)
	message(STATUS "Bouw target voor STM32F412G-Discovery zonder LCD Scherm+Segger EmWin GUI support.")
	set(F412GDiscoveryBibliotheken ${F412GDiscoveryStdBibliotheken})
endif(STM32F412GDiscoveryMetGUI)

add_subdirectory(${ComponentsDir} ${CMAKE_CURRENT_BINARY_DIR}/Components)
add_subdirectory(${F412GDiscoveryDir} ${CMAKE_CURRENT_BINARY_DIR}/F412GDiscoveryLib)
add_subdirectory(${FatFsDir} ${CMAKE_CURRENT_BINARY_DIR}/FatFS)