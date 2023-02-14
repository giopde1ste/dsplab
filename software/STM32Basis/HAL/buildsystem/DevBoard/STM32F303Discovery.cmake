#************************************************************************
# CMake script om STM32F303-Discovery software te compileren.
# Copyright 2004-2017 Hogeschool van Arnhem en Nijmegen
# Opleiding Embedded Systems Engineering
# $Id: STM32F303Discovery.cmake 4390 2022-02-24 14:44:20Z ewout $
#************************************************************************

add_definitions(-DUSE_STM303_DISCOVERY)

set(F303DiscoveryDir ${BSPDir}/STM32F3-Discovery)
 
include_directories(${F303DiscoveryDir} ${UtilitiesDir} ${ComponentsDir})

add_subdirectory(${ComponentsDir} ${CMAKE_CURRENT_BINARY_DIR}/Components)
add_subdirectory(${F303DiscoveryDir} ${CMAKE_CURRENT_BINARY_DIR}/F303DiscoveryLib)

set(F303DiscoveryStdBibliotheken  F3DiscoveryLib ComponentsLib ${STM32F30xStdBibliotheken})
