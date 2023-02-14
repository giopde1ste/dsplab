#/************************************************************************
# CMake script voor wxWidgets CMake software bouwen
# Copyright 2004-2017 Hogeschool van Arnhem en Nijmegen
# Opleiding Embedded Systems Engineering
# $Id: CMakeLists.txt 4389 2022-02-24 14:42:24Z ewout $
#************************************************************************/

if(POLICY CMP0043)
cmake_policy(SET CMP0043 NEW)
endif()

set(wxBASISCPPCHECKAAN 0)

file(GLOB wxBasisBestanden *.cpp)

list(SORT wxBasisBestanden)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (h/desktopAppVersie.h.in "${PROJECT_BINARY_DIR}/gegenereerd/desktopAppVersie.h")

include_directories(h)

if(wxBASISCPPCHECKAAN AND CPPCHECK_FOUND)
set(ARG4 "-I${GEMDIR}/wxBasis/h")
CppCheckRun(wxBasisTest ${GEMDIR}/wxBasis ${PROJECT_BINARY_DIR}/wxBasisTest_testresultaten ${ARG4})
endif(wxBASISCPPCHECKAAN AND CPPCHECK_FOUND)

MaakLib(wxBasis "${wxBasisBestanden}")




