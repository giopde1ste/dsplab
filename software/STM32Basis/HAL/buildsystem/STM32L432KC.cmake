#************************************************************************
# CMake script om STM32 L4 software te compileren.
# Copyright 2012-2018 Hogeschool van Arnhem en Nijmegen
# Opleiding Embedded Systems Engineering
# $Id: STM32L432KC.cmake 4390 2022-02-24 14:44:20Z ewout $
#************************************************************************
 
message(STATUS "Bouw HAL target voor STM32L432KC met 256 KByte Flash en 64K SRAM")

if (NOT GebruikHALDrivers)
	message(FATAL_ERROR "GebruikHALDrivers niet gedefinieerd. STM32L4xx is slecths te gebruiken met de HAL drivers")
endif (NOT GebruikHALDrivers)
	
set(STM32DeviceType stm32l432xx)
add_definitions(-DARMCORTEXM4 -DSTM32L432xx -DSTM32L432 -DSTM32L4)

set(ARMCORTEXM4F ON)

set(stm32L4cmsisdir ${STLIBRARIES}/CMSIS/Device/ST/STM32L4xx)
set(stm32L4haldir ${STLIBRARIES}/STM32L4xx_HAL_Driver)
set(stm32firmwareinclude  ${STLIBRARIES}/CMSIS/Include ${stm32L4cmsisdir}/Include ${stm32L4haldir}/Inc)
set(stm32linkermap ${STM32StdPeriphHALGemeenschappelijkDir}/ldscripts/STM32L432_256k_64k_flash.ld)
set(StartupCode Source/Templates/gcc/startup_stm32L432xx.s)

set (CMAKE_ASM-ATT_FLAGS ${CMAKE_ASM_CM4VLAGGEN})
set (CMAKE_C_FLAGS "${CMAKE_C_CM4FVLAGGEN}")
set (CMAKE_CXX_FLAGS ${CMAKE_CXX_CM4FVLAGGEN})

set (CMAKE_C_LINK_FLAGS "${CMAKE_C_CM4FVLAGGEN} -T \"${stm32linkermap}\" ${StandaardLinkVlaggen}")

#   -Xlinker --defsym=_isatty=_isatty_r
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_C_LINK_FLAGS})

include_directories(${stm32firmwareinclude} ${STM32StdPeriphHALGemeenschappelijkDir}/basis/h)


add_subdirectory(${STM32StdPeriphHALGemeenschappelijkDir}/basis ${CMAKE_CURRENT_BINARY_DIR}/BasisDir)
add_subdirectory(${stm32L4cmsisdir} ${CMAKE_CURRENT_BINARY_DIR}/STM32CMSIS)
add_subdirectory(${stm32L4haldir} ${CMAKE_CURRENT_BINARY_DIR}/STM32_HAL_Driver)

set(Middlewares ${STLIBRARIES}/Middlewares)

set(UtilitiesDir ${STLIBRARIES}/Utilities)

set(BSPDir ${STLIBRARIES}/BSP)
set(ComponentsDir ${BSPDir}/Components)

set(FreeRTOSDir ${ThirdParty}/FreeRTOS)
set(FreeRTOSAddOnsDir ${ThirdParty}/freertos-addons-1.1.0)

set(FatFsLib FatFs_vR0.13a)
set(FatFsDir ${ThirdParty}/${FatFsLib})
#set(FatFsDir ${ThirdParty}/FatFs_vR0.08a)
#set(FatFsDir ${ThirdParty}/FatFs_vR0.13a)



