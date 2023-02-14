#************************************************************************
# CMake script om STM32 f3 software te compileren.
# Copyright 2004-2014 Hogeschool van Arnhem en Nijmegen
# Opleiding Embedded Systems Engineering
# $Id: STM32F301x8.cmake 4390 2022-02-24 14:44:20Z ewout $
#************************************************************************
 
message(STATUS "Bouw HAL target voor STM32F301x8 met 64 KByte Flash en 16K SRAM")

set(STM32DeviceType stm32f301x8)

add_definitions(-DARMCORTEXM4)
add_definitions(-Darm -Dstm32 -Dstm32f30x -DSTM32F30X -DSTM32F301x8)

set(ARMCORTEXM4F ON)

set(stm32f3cmsisdir ${STLIBRARIES}/CMSIS/Device/ST/STM32F3xx)
set(stm32f3haldir ${STLIBRARIES}/STM32F3xx_HAL_Driver)
set(stm32firmwareinclude  ${STLIBRARIES}/CMSIS/Include ${stm32f3cmsisdir}/Include ${stm32f3haldir}/Inc)
set(stm32linkermap ${STM32StdPeriphHALGemeenschappelijkDir}/ldscripts/STM32F30x_64k_16k_flash.ld)
set(StartupCode Source/Templates/gcc/startup_stm32f301x8.s)

set (CMAKE_ASM-ATT_FLAGS ${CMAKE_ASM_CM4VLAGGEN})
set (CMAKE_C_FLAGS "${CMAKE_C_CM4FVLAGGEN}")
set (CMAKE_CXX_FLAGS ${CMAKE_CXX_CM4FVLAGGEN})
set (CMAKE_C_LINK_FLAGS "${CMAKE_C_CM4FVLAGGEN} -T ${stm32linkermap} ${StandaardLinkVlaggen}")
#   -Xlinker --defsym=_isatty=_isatty_r
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_C_LINK_FLAGS})

include_directories(${stm32firmwareinclude} ${STM32StdPeriphHALGemeenschappelijkDir}/basis/h)

add_subdirectory(${STM32StdPeriphHALGemeenschappelijkDir}/basis ${CMAKE_CURRENT_BINARY_DIR}/BasisDir)
add_subdirectory(${stm32f3cmsisdir} ${CMAKE_CURRENT_BINARY_DIR}/STM32CMSIS)
add_subdirectory(${stm32f3haldir} ${CMAKE_CURRENT_BINARY_DIR}/STM32_HAL_Driver)

set(Middlewares ${STLIBRARIES}/Middlewares)

set(UtilitiesDir ${STLIBRARIES}/Utilities)

set(BSPDir ${STLIBRARIES}/BSP)
set(ComponentsDir ${BSPDir}/Components)

set(FreeRTOSDir ${ThirdParty}/FreeRTOS)
set(FreeRTOSAddOnsDir ${ThirdParty}/freertos-addons-1.1.0)
set(FatFsDir ${ThirdParty}/FatFs_vR0.08a)

set(STM32F30xStdBibliotheken ESEBasisLib F3HALDriver CMSISFIRMWARE)
