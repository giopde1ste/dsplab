#  @file 
#  ESE RGT+DSB Practicum BSP bibliotheek.
#  @version $Rev: 3276 $
#  @author $Author: ewout $ 
#  @copyright Copyright 2006-2019 ir drs E.J Boks Hogeschool van Arnhem en Nijmegen
#  $Id: CMakeLists.txt 3276 2019-01-10 11:11:18Z ewout $

message(STATUS "ESE RGT+DSB Board BSP Driver wordt toegevoegd.")

if(NOT BoardVersie)
	message(FATAL_ERROR "BoardVersie moet worden gezet om ESEBoardController.cpp te compileren.")
else(NOT BoardVersie)

	set(SRCS ESEBoardController.cpp ESEBoardComponents.cpp)
	list(SORT SRCS)

	MaakLib(ESE_RGT+DSBBoardLib "${SRCS}")

endif(NOT BoardVersie)