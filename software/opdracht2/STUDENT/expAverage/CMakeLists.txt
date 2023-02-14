#/************************************************************************
# CMake script voor wxWidgets CMake software bouwen.
# Copyright 2004-2019 Hogeschool van Arnhem en Nijmegen
# Opleiding Embedded Systems Engineering
# $Id: CMakeLists.txt 298 2022-02-24 14:43:35Z ewout $
#************************************************************************/

set(SRCS expAverage.cpp)

list(SORT SRCS)

add_library(ExpAverageLib STATIC ${SRCS})