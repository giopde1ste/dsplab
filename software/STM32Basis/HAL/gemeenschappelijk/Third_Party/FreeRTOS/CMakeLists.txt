#  @file 
#  FreeRTOS bibliotheek voor HAN ESE software
#  @version $Rev: 4292 $
#  @author $Author: ewout $ 
#  @note  Werk deze code verder uit volgens de opdracht      
#  @copyright Copyright 2006-2017 ir drs E.J Boks Hogeschool van Arnhem en Nijmegen
#  $Id: CMakeLists.txt 4292 2021-06-22 12:34:00Z ewout $

if (NOT FreeRTOSPortDir)
	message(FATAL_ERROR "FreeRTOS Port directory niet gezet")
else(NOT FreeRTOSPortDir)
	set(FreeRTOSSourceDir ${FreeRTOSDir}/Source)
	file(GLOB SRCS ${FreeRTOSSourceDir}/*.c)
	include_directories(${FRTOSSourceDir}/include ${FreeRTOSPortDir})
	list(APPEND SRCS ${FreeRTOSPortDir}/port.cpp ${FRTOSSourceDir}/CMSIS_RTOS/cmsis_os.c)

	# Gebruik heap methode 4 als standaard
	list(APPEND SRCS ${FreeRTOSSourceDir}/portable/MemMang/heap_4.c)

	list(SORT SRCS)

	message(STATUS "FreeRTOS bibliotheek wordt gecompileerd.")
	MaakLib(FreeRTOSLib "${SRCS}")

endif (NOT FreeRTOSPortDir)
