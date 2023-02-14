/*!
@file
main funktie die behoort bij HAN ESE DSB practicum opdracht 5. /
 main entry point for HAN ESE DSP assignment 5.
@version $Rev: 298 $
@author $Author: ewout $

@copyright Copyright 2017-2019 Hogeschool van Arnhem en Nijmegen, ir drs E.J Boks

$URL: https://ese.han.nl/svn/dsbpracticum/trunk/2022/software/opdracht5/Basis/main.cpp $
$Id: main.cpp 298 2022-02-24 14:43:35Z ewout $
************************************************************************/

#include <ESEBoardController.h>
#include <rtosTaak.h>
#include "student.h"

int main()
{
	/* initialiseer de controller */
	static STM32FilterApp dsb;


	static FilterTask filterTask(dsb);
	static LedBlinkTask ledBlinkTask(dsb);

	Thread::StartScheduler();

	StopHier();  /* hier mogen wij NIET komen */

    return(0);
}


