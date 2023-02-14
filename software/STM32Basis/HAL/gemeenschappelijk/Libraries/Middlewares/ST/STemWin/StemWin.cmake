## Copyright 2011-2016 Hogeschool van Arnhem en Nijmegen
## Opleiding Embedded Systems Engineering
## Ewout Boks
## $Id: StemWin.cmake 4390 2022-02-24 14:44:20Z ewout $
if(NOT ARMCORTEXM4F)
	message(FATAL_ERROR "Voor deze module moet de onderliggende processor een Cortex-M4F zijn.")
else(NOT ARMCORTEXM4F)
	message(STATUS "ST emWin GUI toolkit wordt toegevoegd.")
	include_directories(${StemWinBasisDir}/inc)
	add_library(STemWinLib STATIC IMPORTED )
	# It's important to specify the full path to the library you want to import
	set_target_properties(STemWinLib PROPERTIES IMPORTED_LOCATION ${StemWinBasisDir}/Lib/STemWin532_CM4_GCC.a)
	add_definitions(-DStemWinAanwezig)
endif(NOT ARMCORTEXM4F)