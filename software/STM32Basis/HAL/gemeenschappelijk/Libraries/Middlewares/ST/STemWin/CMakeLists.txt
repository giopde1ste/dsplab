## Copyright 2011-2016 Hogeschool van Arnhem en Nijmegen
## Opleiding Embedded Systems Engineering
## Ewout Boks
## $Id: CMakeLists.txt 4390 2022-02-24 14:44:20Z ewout $

message(STATUS "ST (Segger) EmWin Bibliotheek (STemWin) toegevoegd.")

if(NOT GebruikFreeRTOS)
	add_definitions(-DOS_SUPPORT=0)
	set(SRCS OS/GUI_X.c)
else(NOT GebruikFreeRTOS)
	add_definitions(-DOS_SUPPORT=1)
	set(SRCS OS/GUI_X_OS.c)
endif(NOT GebruikFreeRTOS)

list(SORT SRCS)

include_directories(inc)

MaakLib(STemWinOSLib "${SRCS}")

