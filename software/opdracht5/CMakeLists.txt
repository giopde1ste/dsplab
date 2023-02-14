## Copyright 2011-2021 Hogeschool van Arnhem en Nijmegen
## Opleiding Embedded Systems Engineering
## Ewout Boks
## $Id: CMakeLists.txt 298 2022-02-24 14:43:35Z ewout $

set(PROJEKTNAAM "DSPESEL5")
option(EmbeddedProjekt "Dit is een embedded projekt" ON)

set(VERSIEHOOFDNUMMER 4)
set(VERSIEONDERNUMMER 2)
set(VERSIESUBONDERNUMMER 0)
set(VERSIESUBONDERLETTER a)

cmake_minimum_required(VERSION 3.10.4)

# Voer toolchain ontdekking uit VOOR het projekt wordt gezet.
include(${CMAKE_CURRENT_LIST_DIR}/../STM32Basis/ZetARMCortexMToolchain.cmake)

project(${PROJEKTNAAM})

set(BoardVersie 11)

include_directories(Basis/h)
 
option(OverrideGCC_G++ "Gebruik C++ voor C" ON)
option(GebruikHALDrivers "Gebruik ST HAL bibliotheek" ON)

set(SoftwareBasisDir ${PROJECT_SOURCE_DIR}/..)
set(STM32BasisDir ${SoftwareBasisDir}/STM32Basis)

include(${SoftwareBasisDir}/DSB.cmake)

message(STATUS "HAN ESE DSB Practicum opdracht 5")
message(STATUS "STM32F112G-Discovery FIR Filter applikatie.")

include(${STM32BasisDir}/STM32Development.cmake)

option(STM32F412GDiscoveryMetGUI "Gebruik Segger GUI Driver" ON)
option(GebruikFreeRTOS "Gebruik FreeRTOS Driver" ON)
option(GebruikOOFreeRTOS "GebruikFreeRTOS OO Driver" ON)

include_directories(config)

# Voeg hier de processor specs eerst in. Deze staan los van discovery.cmake ivm invoegen freertos termen.
include(${CMAKESCRIPTDIR}/STM32F412xG.cmake)
include(${FreeRTOSDir}/FreeRTOS.cmake)
include(${CMAKESCRIPTDIR}/DevBoard/STM32F412GDiscovery.cmake)
include(${STM32BasisDir}/STM32PostSetupDevelopment.cmake)

set(BSPDir ${ThirdParty}/ESE_BSP)
option(ESE_RGT+DSBProcessor "Gebruik de ESE RGT+DSB Processor driver" ON)
option(ESE_DigProcesProcessor "Gebruik de ESE Digitaal Proces Processor driver" OFF)
include(${BSPDir}/ESE_BSP.cmake)

MaakSeggerSimpelOzoneConfigCortexM4F(${PROJEKTNAAM}.elf STM32F412ZG "${SVDDir}/STM32F412xG.svd")

# voeg opdracht1 toe aan de bronbestanden
include_directories(${firfilterdir} ${OPDRACHT1DIR}/STUDENT/h)
add_subdirectory(${OPDRACHT1DIR} ${CMAKE_CURRENT_BINARY_DIR}/Opdracht1)

# voeg opdracht 4 toe aan de bronbestanden
set(firfilterdir ${OPDRACHT4DIR}/STUDENT/filter)

add_subdirectory(${firfilterdir} ${CMAKE_CURRENT_BINARY_DIR}/filter)
include_directories(${firfilterdir} ${OPDRACHT1DIR}/STUDENT/h)
include_directories(h STUDENT/h)

set(SRCS Basis/main.cpp Basis/rtosTaak.cpp STUDENT/student.cpp $<TARGET_OBJECTS:DSBKlassen> ${EmWinSTM32F412GDiscoveryDriverSRCS} )
list(SORT SRCS)

# Doel wordt hier samengesteld.
add_executable(${PROJEKTNAAM}.elf ${SRCS} ${BasisSRCS})

target_link_libraries(${PROJEKTNAAM}.elf FirFilterLib ${ESE_BSPLib} ${F412GDiscoveryBibliotheken}  ${FreeRTOSLibs})

if(InterfaceTaalNederlands)
    PostBuildAktie(${PROJEKTNAAM}.elf "STM32 ${PROJEKTNAAM}.elf is gemaakt.")
else(InterfaceTaalNederlands)
    PostBuildAktie(${PROJEKTNAAM}.elf "STM32 ${PROJEKTNAAM}.elf was generated.")
endif(InterfaceTaalNederlands)

