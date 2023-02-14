#/************************************************************************
# CMake script voor wxWidgets CMake software bouwen.
# Copyright 2004-2021 Hogeschool van Arnhem en Nijmegen
# Opleiding Embedded Systems Engineering
# $Id: CMakeLists.txt 313 2023-01-30 13:54:35Z ewout $
#************************************************************************/

set(PROJEKTNAAM "DSPESEL4")

include(${CMAKE_CURRENT_SOURCE_DIR}/../DesktopBasis/buildsystem/InitDesktop.cmake)

set(VERSIEHOOFDNUMMER 4)
set(VERSIEONDERNUMMER 2)
set(VERSIESUBONDERNUMMER 0)
set(VERSIESUBONDERLETTER a)

cmake_minimum_required(VERSION 3.10.4)

project(${PROJEKTNAAM})

include(${PROJECT_SOURCE_DIR}/../DSB.cmake)

set(TAALDIR ${PROJECT_SOURCE_DIR}/../talen)
include(${CMAKESCRIPTDIR}/ZetTalen.cmake)
InstalleerTalen("DSB" ${PROJEKTNAAM} ${TAALDIR})

# [Nederlands] Schakel onderstaand veld in voor extra werk in de opdracht
# Dit leidt tot een hoger cijfer voor de opdracht
# vraag de docent voor de mogelijkheden
# [English] Switch on the field below for extra work in the assignment
# This leads to a higher grade for the assignment
# ask the teacher for the possibilities
add_compile_definitions(ExtraOpties)

# voeg opdracht1 toe aan de bronbestanden
include_directories(${OPDRACHT1DIR}/STUDENT/h)
add_subdirectory(${OPDRACHT1DIR} ${CMAKE_CURRENT_BINARY_DIR}/Opdracht1/STUDENT)

list(SORT SRCS)

include_directories(Basis/h)
include_directories(STUDENT/filter)
include_directories(STUDENT/filter/h)
include_directories(STUDENT/ontwerper)
include_directories(STUDENT/ontwerper/h)

# voeg opdracht 4 applikatie toe
add_subdirectory(Basis)
# Voeg opdracht 4 ontwerper en filter toe
add_subdirectory(STUDENT/filter)
add_subdirectory(STUDENT/ontwerper)
