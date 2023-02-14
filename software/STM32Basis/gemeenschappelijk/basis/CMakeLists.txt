#  @file 
#  ESE Basis Practicum BSP bibliotheek.
#  @version $Rev: 2972 $
#  @author $Author: ewout $ 
#  @copyright Copyright 2006-2017 ir drs E.J Boks Hogeschool van Arnhem en Nijmegen 
#  $Id: CMakeLists.txt 2972 2018-06-08 17:52:50Z ewout $

message(STATUS "Basis bibliotheek wordt gecompileerd.")

set(SRCS CortexMProcessor.cpp)

list(SORT SRCS)

include_directories(h)

MaakLib(ESEBasisLib "${SRCS}")
