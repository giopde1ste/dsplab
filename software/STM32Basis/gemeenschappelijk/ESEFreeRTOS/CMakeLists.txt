#  @file 
#  ESE RGT+DSB Practicum BSP bibliotheek.
#  @version $Rev: 2933 $
#  @author $Author: ewout $ 
#  @copyright Copyright 2006-2018 ir drs E.J Boks Hogeschool van Arnhem en Nijmegen 
#  $Id: CMakeLists.txt 2933 2018-03-21 15:03:26Z ewout $

message(STATUS "ESE FreeRTOS ondersteuning wordt toegevoegd.")

set(SRCS stdTaak.cpp)

list(SORT SRCS)

MaakLib(ESEFreeRTOSLib "${SRCS}")
