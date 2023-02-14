#************************************************************************
# CMake script om STM32 F10x software te compileren.
# Copyright 2004-2014 Hogeschool van Arnhem en Nijmegen
# Opleiding Embedded Systems Engineering
# $Id: STM32F1.cmake 4390 2022-02-24 14:44:20Z ewout $
#************************************************************************

 message(STATUS "Bouw target voor STM32F10x met 1 MByte Flash en 64K SRAM")

add_definitions(-DARMCORTEXM3 -D__FPU_PRESENT=0 -D__FPU_USED=0)
add_definitions(-Darm -Dstm32f10x -DSTM32F1XX -DSTM32F10X_MD)

set(stm32firmwareinclude ${STLIBRARIES}/CMSIS/CM3/DeviceSupport/ST/STM32F10x ${STLIBRARIES}/CMSIS/CM3/CoreSupport ${STLIBRARIES}/STM32F10x_StdPeriph_Driver/inc )

set(stm32linkermap ${STLIBRARIES}/ldscripts/STM32F10x_128k_20k_flash.ld)

set(CMAKE_ASM-ATT_FLAGS ${CMAKE_ASM_CM3VLAGGEN})
set(CMAKE_C_FLAGS "${CMAKE_C_CM3VLAGGEN}")
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_CM3VLAGGEN})

include_directories(${stm32firmwareinclude})

add_subdirectory(${STLIBRARIES}/CMSIS/CM3 ${CMAKE_CURRENT_BINARY_DIR}/CM3)
add_subdirectory(${STLIBRARIES}/STM32F10x_StdPeriph_Driver ${CMAKE_CURRENT_BINARY_DIR}/STM32F10x_StdPeriph_Driver)

if (USE_USBDEVICE)

 message(STATUS "STM32 USB FS Device driver will be included")
 include_directories(${STLIBRARIES}/STM32_USB-FS-Device_Driver/inc)
 add_subdirectory(${STLIBRARIES}/STM32_USB-FS-Device_Driver ${CMAKE_CURRENT_BINARY_DIR}/STM32_USB-FS-Device_Driver)
endif()

set(CMAKE_C_LINK_FLAGS " ${CMAKE_C_CM3VLAGGEN} ${StandaardLinkVlaggen} -T ${stm32linkermap} ${StandaardLinkVlaggen}")
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_C_LINK_FLAGS})

