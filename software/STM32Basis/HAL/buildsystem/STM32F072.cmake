#************************************************************************
# CMake script om STM32 F0 software te compileren.
# Copyright 2004-2018 Hogeschool van Arnhem en Nijmegen
# Opleiding Embedded Systems Engineering
# $Id: STM32F072.cmake 4390 2022-02-24 14:44:20Z ewout $
#************************************************************************
 
message(STATUS "Bouw HAL target voor STM32F072 met 128 kByte Flash en 16K SRAM")

set(STM32DeviceType stm32f072)
add_definitions(-DARMCORTEXM0 -DSTM32F0x -DSTM32F072 -DSTM32F072xB)

set(ARMCORTEXM0 ON)

set(stm32f0cmsisdir ${STLIBRARIES}/CMSIS/Device/ST/STM32F0xx)
set(stm32f0haldir ${STLIBRARIES}/STM32F0xx_HAL_Driver)
set(stm32firmwareinclude  ${STLIBRARIES}/CMSIS/Include ${stm32f0cmsisdir}/Include ${stm32f0haldir}/Inc)
set(stm32linkermap ${STM32StdPeriphHALGemeenschappelijkDir}/ldscripts/STM32F0x_128k_16k_flash.ld)
set(StartupCode Source/Templates/gcc/startup_stm32f072xb.s)

set (CMAKE_ASM-ATT_FLAGS ${CMAKE_ASM_CM0VLAGGEN})
set (CMAKE_C_FLAGS "${CMAKE_C_CM0VLAGGEN}")
set (CMAKE_CXX_FLAGS ${CMAKE_CXX_CM0VLAGGEN})
set (CMAKE_C_LINK_FLAGS "${CMAKE_C_CM0VLAGGEN} -T ${stm32linkermap} ${StandaardLinkVlaggen}")
#   -Xlinker --defsym=_isatty=_isatty_r
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_C_LINK_FLAGS})

include_directories(${stm32firmwareinclude} ${STM32StdPeriphHALGemeenschappelijkDir}/basis/h)

add_subdirectory(${STM32StdPeriphHALGemeenschappelijkDir}/basis ${CMAKE_CURRENT_BINARY_DIR}/BasisDir)
add_subdirectory(${stm32f0cmsisdir} ${CMAKE_CURRENT_BINARY_DIR}/STM32CMSIS)
add_subdirectory(${stm32f0haldir} ${CMAKE_CURRENT_BINARY_DIR}/STM32_HAL_Driver)

set(UtilitiesDir ${STLIBRARIES}/Utilities)

set(BSPDir ${STLIBRARIES}/BSP)
set(ComponentsDir ${BSPDir}/Components)

set(FreeRTOSDir ${ThirdParty}/FreeRTOS)
set(FreeRTOSAddOnsDir ${ThirdParty}/freertos-addons-1.1.0)






