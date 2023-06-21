#/************************************************************************
# CMake script voor wxWidgets CMake software bouwen.
# Copyright 2004-2019 Hogeschool van Arnhem en Nijmegen
# Opleiding Embedded Systems Engineering
# $Id: CMakeLists.txt 328 2023-06-02 11:37:30Z ewout $
#************************************************************************/

cmake_minimum_required(VERSION 3.10.4)

if(NOT PROJEKTNAAM)

	include(${CMAKE_CURRENT_SOURCE_DIR}/../DesktopBasis/buildsystem/InitDesktop.cmake)

	set(PROJEKTNAAM "DSPESEL1")
	
	project(${PROJEKTNAAM})

	include(${PROJECT_SOURCE_DIR}/../DSB.cmake)
	message("HAN ESE DSB practicum opdracht 1.")

	include_directories(Basis/h STUDENT/h)

	add_subdirectory(Basis)

	set(SRCS STUDENT/dsbComplex.cpp STUDENT/dsbComplexBasis.cpp)

	if(UNIX)
		if(APPLE)
			set(MACOSX_BUNDLE_INFO_STRING ${PROJEKTNAAM})
			add_executable(${PROJEKTNAAM} MACOSX_BUNDLE ${SRCS} ${OSX_BUNDLE_ICON})
		else(APPLE)
			add_executable(${PROJEKTNAAM} ${SRCS})
		endif(APPLE)
	endif(UNIX)

	if(WIN32)
		add_executable(${PROJEKTNAAM} WIN32 ${SRCS})
	endif(WIN32)

	target_link_libraries(${PROJEKTNAAM} ApplikatieLib wxBasis ${wxWidgets_LIBRARIES})

	PostBuildAktie(${PROJEKTNAAM})

else(NOT PROJEKTNAAM)

	message(STATUS "Opdracht 1 wordt als DSBKlassen Lib gebouwd.")

	include_directories(STUDENT/h)

	list(APPEND SRCS STUDENT/dsbComplex.cpp STUDENT/dsbComplexBasis.cpp)

	add_library(DSBKlassen OBJECT ${SRCS})

endif(NOT PROJEKTNAAM)
