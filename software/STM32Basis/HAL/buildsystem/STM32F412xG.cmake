#************************************************************************
# CMake script om STM32 F4 software te compileren.
# Copyright 2004-2014 Hogeschool van Arnhem en Nijmegen
# Opleiding Embedded Systems Engineering
# $Id: STM32F412xG.cmake 4390 2022-02-24 14:44:20Z ewout $
#************************************************************************
 
message(STATUS "Bouw HAL target voor STM32F412xG met 1 MByte Flash en 256K SRAM")

set(STM32DeviceType stm32f412xG)
add_definitions(-DARMCORTEXM4 -DSTM32F41x -DSTM32F412 -DSTM32F412Zx)

set(ARMCORTEXM4F ON)

set(stm32f4cmsisdir ${STLIBRARIES}/CMSIS/Device/ST/STM32F4xx)
set(stm32f4haldir ${STLIBRARIES}/STM32F4xx_HAL_Driver)
set(stm32firmwareinclude  ${STLIBRARIES}/CMSIS/Include ${stm32f4cmsisdir}/Include ${stm32f4haldir}/Inc)
set(stm32linkermap ${STM32StdPeriphHALGemeenschappelijkDir}/ldscripts/STM32F412_1024k_256k_flash.ld)
set(StartupCode Source/Templates/gcc/startup_stm32f412zx.s)

set (CMAKE_ASM-ATT_FLAGS ${CMAKE_ASM_CM4VLAGGEN})
set (CMAKE_C_FLAGS "${CMAKE_C_CM4FVLAGGEN}")
set (CMAKE_CXX_FLAGS ${CMAKE_CXX_CM4FVLAGGEN})
set (CMAKE_C_LINK_FLAGS "${CMAKE_C_CM4FVLAGGEN} -T \"${stm32linkermap}\" ${StandaardLinkVlaggen}")
#   -Xlinker --defsym=_isatty=_isatty_r
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_C_LINK_FLAGS})

include_directories(${stm32firmwareinclude} ${STM32StdPeriphHALGemeenschappelijkDir}/basis/h)


add_subdirectory(${STM32StdPeriphHALGemeenschappelijkDir}/basis ${CMAKE_CURRENT_BINARY_DIR}/BasisDir)
add_subdirectory(${stm32f4cmsisdir} ${CMAKE_CURRENT_BINARY_DIR}/STM32CMSIS)
add_subdirectory(${stm32f4haldir} ${CMAKE_CURRENT_BINARY_DIR}/STM32_HAL_Driver)

set(Middlewares ${STLIBRARIES}/Middlewares)
set(StemWinBasisDir  ${Middlewares}/ST/STemWin)

set(UtilitiesDir ${STLIBRARIES}/Utilities)

set(BSPDir ${STLIBRARIES}/BSP)
set(ComponentsDir ${BSPDir}/Components)

set(FreeRTOSDir ${ThirdParty}/FreeRTOS)
set(FreeRTOSAddOnsDir ${ThirdParty}/freertos-addons-1.1.0)

set(FatFsLib FatFs_vR0.13a)
set(FatFsDir ${ThirdParty}/${FatFsLib})
#set(FatFsDir ${ThirdParty}/FatFs_vR0.08a)
#set(FatFsDir ${ThirdParty}/FatFs_vR0.13a)



