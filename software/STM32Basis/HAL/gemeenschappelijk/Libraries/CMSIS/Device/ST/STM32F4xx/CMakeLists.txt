## Copyright 2011-2016 Hogeschool van Arnhem en Nijmegen
## Opleiding Embedded Systems Engineering
## Ewout Boks
## $Id: CMakeLists.txt 4390 2022-02-24 14:44:20Z ewout $

if(NOT StartupCode)
	message(FATAL_ERROR "StartupCode moet wijzen naar het goede startup.s bestand.")
else()
	file(GLOB CoreSupport Source/Templates/*.c)
	message(STATUS "ARM CMSIS support voor STM32F4 wordt toegevoegd.")
	set(SRCS ${CoreSupport} ${StartupCode})
list(SORT SRCS)

MaakLib(CMSISFIRMWARE "${SRCS}")
endif(NOT StartupCode)
