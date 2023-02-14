## Copyright 2011-2016 Hogeschool van Arnhem en Nijmegen
## Opleiding Embedded Systems Engineering
## Ewout Boks
## $Id: BasisF4.cmake 4390 2022-02-24 14:44:20Z ewout $

# ST package voor STM32F4 Discovery board
set(BasisSRCS ../syscalls.cpp term_io.c)

include_directories(${STM32StdPeriphHALGemeenschappelijkDir}/basis)

