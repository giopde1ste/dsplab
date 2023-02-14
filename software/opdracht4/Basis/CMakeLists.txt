#/************************************************************************
# CMake script voor wxWidgets CMake software bouwen
# Copyright 2004-2017 Hogeschool van Arnhem en Nijmegen
# Opleiding Embedded Systems Engineering
# $Id: CMakeLists.txt 298 2022-02-24 14:43:35Z ewout $
#************************************************************************/

set(SRCS applikatie4.cpp)

MaakLib(ApplikatieLib "${SRCS}")
#add_library(ApplikatieLib STATIC ${SRCS})



