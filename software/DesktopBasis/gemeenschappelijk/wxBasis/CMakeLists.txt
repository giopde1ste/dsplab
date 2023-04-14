#/************************************************************************
# CMake script voor wxWidgets CMake software bouwen
# Copyright 2004-2017 Hogeschool van Arnhem en Nijmegen
# Opleiding Embedded Systems Engineering
# $Id: CMakeLists.txt 4685 2023-03-08 15:50:32Z ewout $
#************************************************************************/

if(POLICY CMP0043)
cmake_policy(SET CMP0043 NEW)
endif()

set(wxBASISCPPCHECKAAN 0)

set(wxBasisBestanden desktopApp.cpp grafiekVenster.cpp tinyexpr.c algemeen.cpp)

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




