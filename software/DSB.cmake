#/************************************************************************
# Basis CMake script voor DSB practicum
# Copyright 2004-2022 Hogeschool van Arnhem en Nijmegen
# Hogeschool van Arnhem en Nijmegen
# Opleiding Embedded Systems Engineering
# $Id: DSB.cmake 313 2023-01-30 13:54:35Z ewout $
#************************************************************************/

## BELANGRIJK ..... IMPORTANT ........
## Zet hier de gewenste taal :
## Select the preferred language :
set(InterfaceTaal Nederlands CACHE STRING "Kies voor het Nederlandstalige practicum")   # Voor Nederlands
#set(InterfaceTaal English CACHE STRING "Perform the practical work in English" )  # For English

if (NOT InterfaceTaal)
	message(FATAL_ERROR "Geen taal gekozen : zet de voorkeurstaal!\nNo language selected. Please set the preferred language!\n")
else(NOT InterfaceTaal)
	add_compile_definitions(InterfaceTaal${InterfaceTaal})
endif (NOT InterfaceTaal)

if(InterfaceTaal STREQUAL Nederlands)
	message("HAN Embedded Systems Engineering DSPESEL/DSBPr practicum.")
else(InterfaceTaal STREQUAL Nederlands)
	message("HAN Embedded Systems Engineering DSPESEL/DSBPr practical work.")
endif(InterfaceTaal STREQUAL Nederlands)

message("Docent: ir drs E.J Boks (ewout.boks@han.nl)")

set(VAK DSB CACHE STRING "DSB practicum")
set(VERSIENUMMER "4.1" CACHE STRING "Versie")

option(CCACHEGEBRUIKT  "Gebruik CCache" OFF)
option(CPPCHECKAAN  "Gebruik CPP Check" OFF)
option(OverrideGCC_G++ "Gebruik C++ voor C" ON)

if(POLICY CMP0040)
	cmake_policy(SET CMP0040 NEW)
endif()

if(POLICY CMP0043)
	cmake_policy(SET CMP0043 NEW)
endif()

set(Opdracht "DSBOpdracht")
set(OPDRACHT1DIR ${PROJECT_SOURCE_DIR}/../opdracht1)

if (DesktopProjekt)

	option(CUSTOMPAKKETBESCHRIJVING "Zet aangepaste pakketbeschrijving aan" ON)

	set(CPACK_PACKAGE_NAME ${PROJEKTNAAM})
	set(CPACK_PACKAGE_VENDOR "Hogeschool van Arnhem en Nijmegen, ir drs E.J Boks")
	set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Dit is een demonstratieversie van software die bij het vak Digitale Signaalbewerking van de opleiding Embedded Systems Engineering van de HAN wordt gebruikt.")
	set(CPACK_PACKAGE_VERSION "${VERSIENUMMER}")

	set(INSTALLATIEDIR ${PROJECT_SOURCE_DIR}/..)
	set(CPACK_RESOURCE_FILE_WELCOME ${INSTALLATIEDIR}/${VAK}/welkom.txt)
	set(CPACK_RESOURCE_FILE_README ${INSTALLATIEDIR}/${VAK}/beschrijving.txt)
	set(CPACK_RESOURCE_FILE_LICENSE ${INSTALLATIEDIR}/${VAK}/licentie.txt)

	# opdrachten 1-4 zijn wxWidgets gebaseerde PC opdrachten
	# message(STATUS "Desktop software")
	set(DesktopBasisRelDir ${PROJECT_SOURCE_DIR}/../DesktopBasis)
	# Maak absoluut voor minder lang pad, vooral op windoze van belang.
	get_filename_component(DesktopBasisDir "${DesktopBasisRelDir}"
	                       REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")
	set(GEMDIR ${DesktopBasisDir}/gemeenschappelijk)
	set(CMAKESCRIPTDIR ${DesktopBasisDir}/buildsystem)

	set(VersieString "${VERSIENUMMER}")

	include(${CMAKESCRIPTDIR}/BasisWXWidgets.cmake)
	add_subdirectory(${GEMDIR}/wxBasis ${CMAKE_CURRENT_BINARY_DIR}/wxBasis)
	include_directories(${GEMDIR}/wxBasis/h ${OPDRACHT1DIR})

else(DesktopProjekt)
	if(NOT EmbeddedProjekt)
		message(FATAL_ERROR "DesktopProjekt OF EmbeddedProjekt moet zijn gedefinieerd.")
	else(NOT EmbeddedProjekt)
		# opdracht 5 is een STM32 opdracht
		# message(STATUS "Embedded software")
		set(OPDRACHT4DIR ${PROJECT_SOURCE_DIR}/../opdracht4)

		option(GebruikHALDrivers "Gebruik ST HAL bibliotheek" ON)
	endif(NOT EmbeddedProjekt)
endif(DesktopProjekt)

message(STATUS "InterfaceTaal : ${InterfaceTaal}")
configure_file (${CMAKE_CURRENT_LIST_DIR}/DSBConfig.h.in "${PROJECT_BINARY_DIR}/gegenereerd/DSBConfig.h")
option(OnderwijsOntwikkeling "Schakel Onderwijsontwikkeling mogelijkheden" OFF)
if(OnderwijsOntwikkeling)
	message(WARNING "Onderwijsontwikkeling bezig.")
	add_compile_definitions(OnderwijsOntwikkeling)
endif(OnderwijsOntwikkeling)
	
