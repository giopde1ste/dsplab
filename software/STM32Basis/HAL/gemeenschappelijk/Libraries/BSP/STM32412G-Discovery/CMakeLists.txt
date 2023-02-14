## Boombaard generated CMakelists add_library file
## File generated on : 2017-07-01 14:31:26.
## (c)2002-2017 Kiwanda Embedded Systemen
## $Id: CMakeLists.txt 4390 2022-02-24 14:44:20Z ewout $

set(LibName STM32412G-DiscoveryBasis)

set(SrcPath0 ${CMAKE_CURRENT_SOURCE_DIR}/.)

include_directories(. h)

set(SRCS stm32412g_discovery.c stm32412g_discovery_audio.c stm32412g_discovery_eeprom.c stm32412g_discovery_lcd.c
    stm32412g_discovery_qspi.c stm32412g_discovery_sd.c stm32412g_discovery_ts.c
    STM32412gDiscovery.cpp )

if(GebruikFreeRTOS)
	if(STM32F412GDiscoveryMetGUI)
		list(APPEND SRCS STM32412gDiscoveryGUI.cpp)
	endif(STM32F412GDiscoveryMetGUI)
endif(GebruikFreeRTOS)

list(SORT SRCS)

# Let op Dit is een object library, geen standaard static library
add_library(${LibName} OBJECT ${SRCS})

## This library can now be included in any build by using "add_subdirectory"( the_path_to_this_lib ).

