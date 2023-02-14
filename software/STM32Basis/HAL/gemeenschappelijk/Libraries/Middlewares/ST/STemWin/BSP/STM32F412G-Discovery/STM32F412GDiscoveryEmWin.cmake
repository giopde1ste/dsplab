## Boombaard generated STM32F212GDiscoveryEmWin include file
## File generated on : 2017-06-30 15:34:38.
## (c)2002-2017 Kiwanda Embedded Systemen
## $Id: STM32F412GDiscoveryEmWin.cmake 4390 2022-02-24 14:44:20Z ewout $

set(SrcPath0 ${CMAKE_CURRENT_LIST_DIR}/.)

include_directories(${SrcPath0})

set(EmWinSTM32F412GDiscoveryDriverSRCS ${SrcPath0}/GUIConf_stm32412g_discovery.c
    ${SrcPath0}/LCDConf_stm32412g_discovery.c
    ${SrcPath0}/standaardGrafiekVenster.cpp)

list(SORT EmWinSTM32F412GDiscoveryDriverSRCS)

## Include this file where you would want to compile the listed source files (stored in variable SRCS).
## and for including the listed include_directories directives.

