#  @file 
#  ESE RGT+DSB Practicum BSP bibliotheek.
#  @version $Rev: 2607 $
#  @author $Author: ewout $ 
#  @copyright Copyright 2006-2017 ir drs E.J Boks Hogeschool van Arnhem en Nijmegen 
#  $Id: CMakeLists.txt 2607 2017-03-10 16:48:42Z ewout $

message(STATUS "ESE DigProces Board BSP Driver wordt toegevoegd.")

set(SRCS ESEDigProcesController.cpp)

list(SORT SRCS)

include_directories(.)

MaakLib(ESE_DigProcesLib "${SRCS}")
