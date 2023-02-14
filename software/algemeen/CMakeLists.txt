#  @file 
#  ESE Basis Practicum BSP bibliotheek.
#  @version $Rev: 2975 $
#  @author $Author: ewout $ 
#  @copyright Copyright 2006-2017 ir drs E.J Boks Hogeschool van Arnhem en Nijmegen 
#  $Id: CMakeLists.txt 2975 2018-06-13 11:09:35Z ewout $

message(STATUS "Algemeen bibliotheek wordt gecompileerd.")

set(SRCS algdef.cpp dataPakket.cpp)

list(SORT SRCS)

include_directories(h)

MaakLib(AlgemeenLib ${SRCS})
