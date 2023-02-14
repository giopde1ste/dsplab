## Copyright 2011-2016 Hogeschool van Arnhem en Nijmegen
## Opleiding Embedded Systems Engineering
## Ewout Boks
## $Id: CMakeLists.txt 4390 2022-02-24 14:44:20Z ewout $

message(STATUS "STM32F4xx Utilities Driver wordt toegevoegd.")

FILE (GLOB_RECURSE SRCS *.c)
include_directories()

list(SORT SRCS)

add_library(UtilitiesLib ${SRCS})





