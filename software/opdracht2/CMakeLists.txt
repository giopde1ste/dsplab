#/************************************************************************
# CMake script voor wxWidgets CMake software bouwen.
# Copyright 2004-2021 Hogeschool van Arnhem en Nijmegen
# Opleiding Embedded Systems Engineering
# $Id: CMakeLists.txt 313 2023-01-30 13:54:35Z ewout $
#************************************************************************/

set(PROJEKTNAAM "DSPESEL2")

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

include_directories(STUDENT/expAverage)
add_subdirectory(STUDENT/expAverage)

# voeg opdracht1 toe aan de bronbestanden
include_directories(${OPDRACHT1DIR}/STUDENT/h)
add_subdirectory(${OPDRACHT1DIR} ${CMAKE_CURRENT_BINARY_DIR}/Opdracht1)

set(SRCS STUDENT/gemwaarde.cpp $<TARGET_OBJECTS:DSBKlassen>)

if(UNIX)
	if(APPLE)
		set(MACOSX_BUNDLE_INFO_STRING "HAN ESE DSB opdracht2")
		set(OSX_BUNDLE_ICON ${PROJECT_SOURCE_DIR}/platforms/apple/${PROJEKTNAAM}.icns)
		add_executable(${PROJEKTNAAM} MACOSX_BUNDLE ${SRCS} ${OSX_BUNDLE_ICON} ${TALEN})

		ConfigureMacOSXBundlePlist(${PROJEKTNAAM} ${StdCopyrightString} ${OSX_BUNDLE_ICON} ${VersieString})
		ConfigureMacOSXPack(${PROJEKTNAAM} ${OSX_BUNDLE_ICON} "10.7")

		install(TARGETS ${PROJEKTNAAM} RUNTIME DESTINATION bin BUNDLE DESTINATION .)
	else(APPLE)
		add_executable(${PROJEKTNAAM} ${SRCS})
		
		install(TARGETS ${PROJEKTNAAM} RUNTIME DESTINATION bin
	        PERMISSIONS
	        OWNER_WRITE OWNER_READ OWNER_EXECUTE
	        GROUP_READ GROUP_EXECUTE
	        WORLD_READ WORLD_EXECUTE)
	endif(APPLE)
endif(UNIX)

if(WIN32)
	#list(APPEND SRCS ${PROJECT_SOURCE_DIR}/platforms/windows/${PROJEKTNAAM}.rc)

	add_executable(${PROJEKTNAAM} WIN32 ${SRCS})
	
	file(GLOB MSRUNTIME ${GEMDIR}/win32/ms*)
	install(FILES ${MSRUNTIME} DESTINATION bin)
	
	install(TARGETS ${PROJEKTNAAM} RUNTIME DESTINATION bin
	        PERMISSIONS
	        OWNER_WRITE OWNER_READ OWNER_EXECUTE
	        GROUP_READ GROUP_EXECUTE
	        WORLD_READ WORLD_EXECUTE)
endif(WIN32)

target_link_libraries(${PROJEKTNAAM} ExpAverageLib wxBasis ${wxWidgets_LIBRARIES})

include(${CMAKESCRIPTDIR}/FixBundle.cmake)

PostBuildAktie(${PROJEKTNAAM})