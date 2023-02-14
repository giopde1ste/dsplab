#  @file 
#  FreeRTOS bibliotheek voor HAN ESE software
#  @version $Rev: 4292 $
#  @author $Author: ewout $ 
#  @note  Werk deze code verder uit volgens de opdracht      
#  @copyright Copyright 2006-2017 ir drs E.J Boks Hogeschool van Arnhem en Nijmegen
#  $Id: FreeRTOS.cmake 4292 2021-06-22 12:34:00Z ewout $

if(GebruikFreeRTOS)

	add_definitions(-DFreeRTOSGebruikt)

	set(FRTOSSourceDir ${FreeRTOSDir}/Source)

	if(ARMCORTEXM0)
		set(FreeRTOSPortDir ${FRTOSSourceDir}/portable/GCC/ARM_CM0)
		message(STATUS "FreeRTOS ARM Cortex M0 Port gekozen.")
	elseif(ARMCORTEXM3)
		set(FreeRTOSPortDir ${FRTOSSourceDir}/portable/GCC/ARM_CM3)
		message(STATUS "FreeRTOS ARM Cortex M3 Port gekozen.")
	elseif(ARMCORTEXM4F)
		set(FreeRTOSPortDir ${FRTOSSourceDir}/portable/GCC/ARM_CM4F)
		message(STATUS "FreeRTOS ARM Cortex M4F Port gekozen.")
	elseif(ARMCORTEXM7)
		set(FreeRTOSPortDir ${FRTOSSourceDir}/portable/GCC/ARM_CM7)
		message(STATUS "FreeRTOS ARM Cortex M7 Port gekozen.")
	else()
		message(FATAL_ERROR "ARM Cortex type niet gezet")
	endif(ARMCORTEXM0)

	include_directories(${FRTOSSourceDir}/include ${FRTOSSourceDir}/CMSIS_RTOS ${FreeRTOSPortDir})
	add_subdirectory(${FreeRTOSDir} ${CMAKE_CURRENT_BINARY_DIR}/FreeRTOS)

	set(FreeRTOSLibs ESEFreeRTOSLib FreeRTOSLib)
	
	if(GebruikOOFreeRTOS)
		# message(STATUS "FreeRTOS Object Oriented toevoeging wordt gebouwd.")
		set(FreeRTOSOOSourceDir "${ThirdParty}/freertos-addons-1.1.0/c++/Source")
		include_directories(${FreeRTOSOOSourceDir}/include)
		add_subdirectory(${ThirdParty}/freertos-addons-1.1.0 ${CMAKE_CURRENT_BINARY_DIR}/FreeRTOSOOSchil)
		list(INSERT FreeRTOSLibs 0 FreeRTOSOOSchilLib)
	endif(GebruikOOFreeRTOS)
	
	include_directories(${ESEFreeRTOSDir}/h)
	add_subdirectory(${ESEFreeRTOSDir} ${CMAKE_CURRENT_BINARY_DIR}/ESE_FreeRTOS)

endif(GebruikFreeRTOS)
