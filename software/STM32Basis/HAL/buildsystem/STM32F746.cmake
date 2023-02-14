#************************************************************************
# CMake script om STM32 F7 software te compileren.
# Copyright 2004-2017 Hogeschool van Arnhem en Nijmegen
# Opleiding Embedded Systems Engineering
# $Id: STM32F746.cmake 4390 2022-02-24 14:44:20Z ewout $
#************************************************************************
 
message(STATUS "Bouw HAL target voor STM32F4746 met 1 MByte Flash en 320K SRAM")

set(STM32DeviceType stm32f746xx)
add_definitions(-DARMCORTEXM7 -DSTM32F7xx)

set(ARMCORTEXM4F ON)

set(stm32f7cmsisdir ${STLIBRARIES}/CMSIS/Device/ST/STM32F7xx)
set(stm32f7haldir ${STLIBRARIES}/STM32F7xx_HAL_Driver)
set(stm32firmwareinclude  ${STLIBRARIES}/CMSIS/Include ${stm32f7cmsisdir}/Include ${stm32f7haldir}/Inc)
set(stm32linkermap ${STM32StdPeriphHALGemeenschappelijkDir}/ldscripts/STM32F746_1024k_320k_flash.ld)
set(StartupCode Source/Templates/gcc/startup_stm32f746xx.s)

set (CMAKE_ASM-ATT_FLAGS ${CMAKE_ASM_CM7VLAGGEN})
set (CMAKE_C_FLAGS "${CMAKE_C_CM7FVLAGGEN}")
set (CMAKE_CXX_FLAGS ${CMAKE_CXX_CM7FVLAGGEN})
set (CMAKE_C_LINK_FLAGS "${CMAKE_C_CM7FVLAGGEN} -T ${stm32linkermap} ${StandaardLinkVlaggen}")
#   -Xlinker --defsym=_isatty=_isatty_r
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_C_LINK_FLAGS})

include_directories(${stm32firmwareinclude} ${STM32StdPeriphHALGemeenschappelijkDir}/basis/h)

add_subdirectory(${STM32StdPeriphHALGemeenschappelijkDir}/basis ${CMAKE_CURRENT_BINARY_DIR}/BasisDir)
add_subdirectory(${stm32f7cmsisdir} ${CMAKE_CURRENT_BINARY_DIR}/STM32CMSIS)
add_subdirectory(${stm32f7haldir} ${CMAKE_CURRENT_BINARY_DIR}/STM32_HAL_Driver)

set(Middlewares ${STLIBRARIES}/Middlewares)
set(StemWinBasisDir  ${Middlewares}/ST/StemWin)

set(UtilitiesDir ${STLIBRARIES}/Utilities)

set(BSPDir ${STLIBRARIES}/BSP)
set(ComponentsDir ${BSPDir}/Components)

set(FreeRTOSDir ${ThirdParty}/FreeRTOS)
set(FreeRTOSAddOnsDir ${ThirdParty}/freertos-addons-1.1.0)
set(FatFsDir ${ThirdParty}/FatFs_vR0.08a)





