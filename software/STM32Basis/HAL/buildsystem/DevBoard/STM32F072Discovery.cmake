#************************************************************************
# CMake script om STM32F072 Discovery software te compileren.
# Copyright 2004-2018 Hogeschool van Arnhem en Nijmegen
# Opleiding Embedded Systems Engineering
# $Id: STM32F072Discovery.cmake 4390 2022-02-24 14:44:20Z ewout $
#************************************************************************

add_definitions(-DUSE_DiscoverySTM32F072)

set(STM32F072BSPDir ${BSPDir}/STM32F072B-Discovery)

include_directories(${STM32F072BSPDir} ${STM32F072BSPDir}/h ${UtilitiesDir} ${ComponentsDir})

#add_subdirectory(${ComponentsDir} ${CMAKE_CURRENT_BINARY_DIR}/Components)
add_subdirectory(${STM32F072BSPDir} ${CMAKE_CURRENT_BINARY_DIR}/STM32F072BDiscoveryBSDLib)

add_library(DiscoverySTM32F072 $<TARGET_OBJECTS:STM32F072BDiscovery> )
# Zit niet op de Nukleo : $<TARGET_OBJECTS:ComponentsLib>

set(STM32F072DiscoveryStdBibliotheken ESEBasisLib SEGGER_Sysview Segger_RTT DiscoverySTM32F072
    STM32F0xx_HAL_Driver AlgemeenLib CMSISFIRMWARE)

set(STM32F072DiscoveryBibliotheken ${STM32F072DiscoveryStdBibliotheken})

