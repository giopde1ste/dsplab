/******************************************************************************
 * Project        : ES2 Project 3 - Fuzzy Control
 * File           : Fuzzy library
 * Copyright      : 2010-2011 - H. Arends en E.J Boks
 ******************************************************************************
@file 
RTOS taak voor debug digitale signaalbewerking practicum

@version $Rev: 298 $
@author $Author: ewout $ 
                                   
@copyright Copyright 2013-2021 Hogeschool van Arnhem en Nijmegen
         
$Id: rtosTaak.cpp 298 2022-02-24 14:43:35Z ewout $
************************************************************************/

#include <rtosTaak.h>
#include <GUI.h>

FilterTask::FilterTask(STM32FilterApp &fa) : StdTaak("FilterTask",
                                                      1024,
                                                      TaakPrio::Taak2),
                                                      stm32FilterApp(fa)
{
	Start();
}

void FilterTask::Run()
{
	const auto metSkoop = startF412();

	/* laat weten dat het filter begint. */
	const StdTaak::StdMsg bericht(TaakPrio::Taak2,
	                                   TaakPrio::Taak1,
	                                   MsgType::VerbindingGemaakt);
	stuurBericht(bericht);
	wacht(100); /* schakel even over om de led taak het bericht te laten verwerken. */
	stm32FilterApp.hoofdlus(metSkoop);
}

Schakelaar FilterTask::startF412()
{
#ifdef InterfaceTaalNederlands
	static const char *opdrachtTekst = "Opdracht 5:FIR Filter";
	static const char *skoopAanTekst = "Run Filter Test op GUI";
	static const char *skoopUitTekst = "Start het FIR Filter";
	static const char *startTekst = "Druk \"Links\" om te beginnen";
#else
	static const char *opdrachtTekst = "Assignment 5:FIR Filter";
	static const char *skoopAanTekst = "Run Filter Test in GUI";
	static const char *skoopUitTekst = "Start the FIR Filter";
	static const char *startTekst = "Press \"Left\" to start";
#endif

#ifdef USE_STM32412G_DISCOVERY
	auto &controller = stm32FilterApp;
	auto gebruikSkoop = stm32FilterApp.geefBKUPGebruikGUISkoopKeuze();

	controller.setLED(ESEBoardController::LEDKleur::Blue,
	                  (Schakelaar::Aan == gebruikSkoop) ? Schakelaar::Uit : Schakelaar::Aan);
	controller.setLED(ESEBoardController::LEDKleur::Orange,
	                  gebruikSkoop);

	/* initialiseer de GUI */
	controller.guiDriver.postOSStartGUIInit();

	GUI_Clear();
	GUI_SetFont(&GUI_Font20_1);
	GUI_DispStringHCenterAt("ESE DSPESEL practicum",120,8);

	GUI_DispStringHCenterAt(opdrachtTekst,120,30);
	GUI_SetFont(&GUI_FontComic18B_1);
	GUI_SetTextMode( ((Schakelaar::Aan == gebruikSkoop ) ? GUI_TM_NORMAL : GUI_TM_REV));
	GUI_DispStringHCenterAt(skoopUitTekst,120,80);
	GUI_SetTextMode( ((Schakelaar::Uit == gebruikSkoop) ? GUI_TM_NORMAL : GUI_TM_REV));
	GUI_DispStringHCenterAt(skoopAanTekst,120,110);

	GUI_SetTextMode(GUI_TM_NORMAL);
	GUI_DispStringHCenterAt(startTekst,120,150);


	/* schakel de LEDs aan */
	controller.setLED(STM32BoardController::LEDKleur::Green,
	                  Schakelaar::Aan);

	bool wachtOpInstruktie = true;

	do
	{
		/* Wacht tot een knop is ingedrukt. */
		const auto knop = controller.wachtOpEenKnop(false);

		switch (knop)
		{
			case STM32412gDiscovery::KnopID::Boven:
			{
				gebruikSkoop = Schakelaar::Uit;
				controller.setLED(STM32BoardController::LEDKleur::Blue,
				                  Schakelaar::Aan);
				controller.setLED(STM32BoardController::LEDKleur::Orange,
				                  Schakelaar::Uit);
				GUI_SetTextMode( GUI_TM_REV);
				GUI_DispStringHCenterAt(skoopUitTekst,120,80);
				GUI_SetTextMode(GUI_TM_NORMAL);
				GUI_DispStringHCenterAt(skoopAanTekst,120,110);
			};
				break;

			case STM32412gDiscovery::KnopID::Onder:
			{
				gebruikSkoop = Schakelaar::Aan;
				controller.setLED(STM32BoardController::LEDKleur::Blue,
				                  Schakelaar::Uit);
				controller.setLED(STM32BoardController::LEDKleur::Orange,
				                  Schakelaar::Aan);

				GUI_SetTextMode(GUI_TM_NORMAL);
				GUI_DispStringHCenterAt(skoopUitTekst,120,80);
				GUI_SetTextMode( GUI_TM_REV);
				GUI_DispStringHCenterAt(skoopAanTekst,120,110);
			};
				break;

			case STM32412gDiscovery::KnopID::Links:
			{
				wachtOpInstruktie = false;
			};
				break;

			default:
				break;
		}

		HAL_Delay(250);

	} while (true == wachtOpInstruktie);

	controller.zetBKUPGebruikLCDKeuze(gebruikSkoop);
	return(gebruikSkoop);
#else
	static constexpr auto useScope = Schakelaar::Uit;
	return(useScope);
#endif
}


LedBlinkTask::LedBlinkTask(ESEBoardController &fa) : StdTaak("LED Blink task",
                                                          512,
                                                          TaakPrio::Taak1),eseBoardController(fa)
{
	Start();
}

[[noreturn]] void LedBlinkTask::Run()
{
	FOREVER
	{
		const auto nieuw = wachtOpBericht(0);

		if (FoutCode::Ok == nieuw)
		{
			const auto type = geefType();

			switch(type)
			{
				case MsgType::VerbindingGemaakt:
					eseBoardController.knipperBlauwLED(ESEBoardController::KnipperLEDStand::Middel);
					break;

				default:
					StopHier();
					break;
			}
		}
		else
		{
			BSP_LED_Toggle(LED_RED);
		}


	}
}
