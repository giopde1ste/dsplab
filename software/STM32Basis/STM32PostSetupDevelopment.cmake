## Copyright 2011-2017 Hogeschool van Arnhem en Nijmegen
## Opleiding Embedded Systems Engineering
## ir drs E.J Boks
## Basis CMake (HAL) bestand voor STM32 gerelateerde development.
## $Id: STM32PostSetupDevelopment.cmake 4639 2022-11-17 16:42:29Z ewout $

if(NOT SoftwareBasisDir)
	message(FATAL_ERROR "Geef met SoftwareBasisDir aan waar de software basis bibliotheken staan.")
else(NOT STM32BasisDir)
	if(NOT STM32BasisDir  )
		message(FATAL_ERROR "Geef met STM32BasisDir aan waar de STM32 CMake scripts en STM32 bibliotheken staan.")
	else(NOT STM32BasisDir)

		if (GebruikHALDrivers)

		else(GebruikHALDrivers)

		endif(GebruikHALDrivers)

		add_subdirectory(${SoftwareBasisDir}/algemeen ${CMAKE_CURRENT_BINARY_DIR}/AlgemeenDir)

		# Segger RTT
		set(SeggerRTTDir ${STM32BasisDir}/Segger/Segger_RTT)
		include_directories(${SeggerRTTDir})
		add_subdirectory(${SeggerRTTDir} ${CMAKE_CURRENT_BINARY_DIR}/SeggerRTT)
		add_compile_definitions(SeggerRTTAanwezig)
		
		# System View
		# Niet nodig in onderwijs.
		#set(SeggerSysViewDir ${STM32BasisDir}/Segger/Segger_Sysview)
		#include_directories(${SeggerSysViewDir})
		#add_subdirectory(${SeggerSysViewDir} ${CMAKE_CURRENT_BINARY_DIR}/SeggerSysview)
		#add_compile_definitions(SeggerSysViewAanwezig)


	endif(NOT STM32BasisDir)
endif(NOT SoftwareBasisDir)
