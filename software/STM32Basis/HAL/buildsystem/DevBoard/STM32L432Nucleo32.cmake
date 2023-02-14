#************************************************************************
# CMake script om STM32L432Nucleo32 software te compileren.
# Copyright 2004-2018 Hogeschool van Arnhem en Nijmegen
# Opleiding Embedded Systems Engineering
# $Id: STM32L432Nucleo32.cmake 4648 2022-12-08 16:06:11Z ewout $
#************************************************************************

add_definitions(-DUSE_STM32L432Nucleo32)

set(L432Nucleo32Dir ${BSPDir}/STM32L4xx_Nucleo_32)
 
include_directories(${L432Nucleo32Dir} ${L432Nucleo32Dir}/h ${FatFsDir} ${UtilitiesDir} ${ComponentsDir})

#add_subdirectory(${ComponentsDir} ${CMAKE_CURRENT_BINARY_DIR}/Components)
add_subdirectory(${L432Nucleo32Dir} ${CMAKE_CURRENT_BINARY_DIR}/L432NucleoLib)

add_library(STM32L4xxNucleo32 $<TARGET_OBJECTS:STM32L4xxNucleo32Basis> )
# Zit niet op de Nukleo : $<TARGET_OBJECTS:ComponentsLib>

set(L432NucleoStdBibliotheken STM32L4xxNucleo32 ESEBasisLib AlgemeenLib Segger_RTT STM32L4xx_HAL_Driver CMSISFIRMWARE)

set(L432NucleoBibliotheken ${L432NucleoStdBibliotheken})

