#  @file 
#  ESE RGT+DSB Practicum BSP bibliotheek.
#  @version $Rev: 2645 $
#  @author $Author: ewout $ 
#  @copyright Copyright 2006-2017 ir drs E.J Boks Hogeschool van Arnhem en Nijmegen 
#  $Id: CMakeLists.txt 2645 2017-03-28 12:51:40Z ewout $

message(STATUS "ESE DigProces Aansturing API wordt toegevoegd.")

set(SRCS DigProcesAansturing.cpp)

list(SORT SRCS)

include_directories(.)

MaakLib(ESE_DigProcesAansturingLib "${SRCS}")
