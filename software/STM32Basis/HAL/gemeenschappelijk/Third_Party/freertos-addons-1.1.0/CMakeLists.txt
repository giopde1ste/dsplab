#  @file 
#  FreeRTOS C++ Schil bibliotheek voor HAN ESE software
#  @version $Rev: 2669 $
#  @author $Author: ewout $ 
#  @note  Werk deze code verder uit volgens de opdracht      
#  @copyright Copyright 2006-2016 ir drs E.J Boks Hogeschool van Arnhem en Nijmegen 
#  $Id: CMakeLists.txt 2669 2017-04-19 07:34:23Z ewout $

set(SourceDir "c++/Source")
FILE(GLOB SRCS ${SourceDir}/*.c ${SourceDir}/*.cpp)
include_directories(${SourceDir}/include)
 
message(STATUS "FreeRTOS C++ Schil bibliotheek wordt gecompileerd.")
MaakLib(FreeRTOSOOSchilLib "${SRCS}")
