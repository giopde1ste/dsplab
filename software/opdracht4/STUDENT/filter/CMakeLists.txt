#  @file 
#  FirFilter voor opdracht 4 en 5 DSB practicum
#  @version $Rev: 298 $
#  @author $Author: ewout $ 
#  @copyright Copyright 2006-2019 ir drs E.J Boks Hogeschool van Arnhem en Nijmegen
#  $Id: CMakeLists.txt 298 2022-02-24 14:43:35Z ewout $

set(Opdracht1Dir ../../../opdracht1)

include_directories(${Opdracht1Dir})

set(SRCS firfilter.cpp)

list(SORT SRCS)

add_library(FirFilterLib STATIC ${SRCS})
