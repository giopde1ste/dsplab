#  @file 
#  ESE RGT+DSB Practicum BSP bibliotheek.
#  @version $Rev: 3276 $
#  @author $Author: ewout $ 
#  @copyright Copyright 2006-2017 ir drs E.J Boks Hogeschool van Arnhem en Nijmegen 
#  $Id: ESE_BSP.cmake 3276 2019-01-10 11:11:18Z ewout $

if(NOT BoardVersie)
	message(FATAL_ERROR "Definieer in toplevel CMakeLists.txt voor welke boardversie wordt gebouwd.")
else(NOT BoardVersie)
	add_definitions(-DBoardVersie=${BoardVersie})

	message(STATUS "ESE BSP Driver (boardversie ${BoardVersie}) wordt toegevoegd.")

	set(BSPDir ${ThirdParty}/ESE_BSP)

	# gemeenschappelijke dir wordt toegevoegd.
	include_directories(${BSPDir}/DigProcesAansturing/h)
	add_subdirectory(${BSPDir}/DigProcesAansturing ${CMAKE_CURRENT_BINARY_DIR}/ESE_DigProcesAansturing)

	set(ESE_BSPLib ESE_DigProcesAansturingLib)

	if(ESE_RGT+DSBProcessor)
		include_directories(${BSPDir}/ESEPracticumboard_RGT+DSB/h)
		add_subdirectory(${BSPDir}/ESEPracticumboard_RGT+DSB ${CMAKE_CURRENT_BINARY_DIR}/ESE_BSPProcessor)
		list(INSERT ESE_BSPLib 0 ESE_RGT+DSBBoardLib)
	elseif(ESE_DigProcesProcessor)
		include_directories(${BSPDir}/ESEPracticumboard_DigProces/h)
		add_subdirectory(${BSPDir}/ESEPracticumboard_DigProces ${CMAKE_CURRENT_BINARY_DIR}/ESEPracticumboard_DigProces)
		list(INSERT ESE_BSPLib 0 ESE_DigProcesLib)
	else()
		message(FATAL_ERROR "Definieer ESE_RGT+DSBProcessor of ESE_DigProcesProcessor")
	endif()

endif(NOT BoardVersie)

