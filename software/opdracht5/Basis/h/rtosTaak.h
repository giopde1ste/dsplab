/******************************************************************************
 * Project        : HAN ESE Project 3 - Regeltechniek
 * File           : Regeltechniek Taak klasse
 * Copyright      : 2010-2017 HAN Embedded Systems Engineering
 ******************************************************************************
  Change History:

    Version 1.0 - September 2010
    > Initial revision

$Id: rtosTaak.h 298 2022-02-24 14:43:35Z ewout $  
******************************************************************************/

#ifndef HANESE_rtosTaak_H
#define HANESE_rtosTaak_H

#include <stdio.h>
#include <math.h>
#include <assert.h>

#include <thread.hpp>
#include <queue.hpp>
#include <algdef.h>
#include <ESEBoardController.h>
#include <stdTaak.h>
#include <student.h>


using namespace cpp_freertos;

class FilterTask : public StdTaak
{
public:
	/*! @brief Constructor. */
	explicit FilterTask(STM32FilterApp &);

protected:
	void Run() final;

private:
	/* TODO 05.03.2021 Plaats hier een dummie buffer om geheugencorruptie op te vangen.
	 * TODO Echte fix is nog niet aanwezig. */
	UInt32 dummyGeheugen[64];

	Schakelaar startF412();

	STM32FilterApp &stm32FilterApp;
};


class LedBlinkTask : public StdTaak
{
public:
	explicit LedBlinkTask(ESEBoardController &);

protected:
	[[noreturn]] void Run() final;

private:
	ESEBoardController &eseBoardController;
};


#endif
