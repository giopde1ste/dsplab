/*!
@file
Opdracht 5 DSB practicum. Werk deze opdracht verder uit aan de hand van het kommentaar.
@version $Rev: 313 $
@author $Author: ewout $

@copyright Copyright 2006-2020 ir drs E.J Boks, Hogeschool van Arnhem en Nijmegen. https://ese.han.nl/~ewout

$URL: https://ese.han.nl/svn/dsbpracticum/trunk/2022/software/opdracht5/STUDENT/student.cpp $
$Id: student.cpp 313 2023-01-30 13:54:35Z ewout $
************************************************************************/

#ifndef OnderwijsOntwikkeling
#if defined(InterfaceTaalNederlands)
#elif defined (InterfaceTaalEnglish)
/* Remove this directive after filling out name and student number below. */
#error  "Student name and number must be entered into the fields below."
#endif
#endif

/********  Naam/name     :Thomas van den Oever ******/
/********  Studentnummer :1585101              ******/

#include <student.h>
#include <firfilterexport.h>   /* Dit MOET aanwezig zijn in de map / this MUST be present in the directory.  */
#include <RTT/SEGGER_RTT.h>
#include <stdTaak.h>

void STM32FilterApp::runFilter()
{
	/* Werk deze funktie verder uit om er voor te zorgen dat :
		- De bemonstering frequentie van de ADC zodanig ingesteld wordt dat aan de opdrachteisen is voldaan.
		- De ADC start en dat de DA converter ook opgestart is.
	    - Gewacht wordt tot een sample ingelezen is.
	    - Het sample uit de ADC wordt opgehaald en wordt ingeladen in het filter.
	    - Het filter een waarde teruggeeft, welke vervolgens in de DA converter wordt ingeladen.

	    English :
	    Develop this function further to ensure that:
		- The sampling frequency of the ADC is set so that the assignment requirements are met.
		- The ADC starts and that the DA converter is also started.
		- Wait until a sample has been read.
		- The sample from the ADC is retrieved and loaded into the filter.
		- The filter returns a value, which is then loaded into the DA converter.
	 */

//#error "Dit deel van de software ontbreekt — this part of the software is missing."
/* Beste leerling, dit deel van de software ontbreekt. Vul dit deel aan volgens de opdracht.  
   Dear student, this part of the software is missing. Complete this part accoording to the assignment.
*/
    ads131a02.zetSampFreq(ADS131A02::ICLK::ICLK8, ADS131A02::FMOD::FMOD8, ADS131A02::ODR::ODR64);
    ads131a02.start();
    max5136.start(DSB_DAC_Channel);
while(1) {
    ads131a02.wachtOpDataReady();
    ADS131A02::ADCData data;
    ads131a02.laadConversieData();
    auto spanning = ads131a02.konverteer(DSB_ADC_Channel) * 10.0f;
    Int16 filterdata;
    filterdata = filter.filter(spanning);
    auto DACspanning = max5136.dacSpanning(filterdata);
    max5136.zetSpanning(DSB_DAC_Channel, DACspanning);
}

	/* Hier mag de uitvoering niet komen! / execution should not reach this point! */
	StopHier();

}

void STM32FilterApp::runGUITest()
{
	/*! @note Configureer deze waarden om de juiste simulatie te krijgen, if necessary. De standaardwaarden zijn geschikt voor
	 * simulatie van de opdracht 4 filterexport.
	 * Configure these values to obtain the right simulation, if necessary. The default values are suitable for simulation
	 * of  assignment 4 filter export. */
	static constexpr auto sampFreq = 4000.0f; /* Hz */
	static constexpr auto beginSweep= 100.0f; /* Hz */
	static constexpr auto endSweep = 1000.0f; /* Hz */
	static constexpr auto sweepIncr = 100.0f; /* Hz */
	static constexpr auto sweepNumber = 2*AantalTaps;  /* samples /sweep */

	auto n = 0;
	auto freq = beginSweep;
	do
	{
		const auto omega = (TweePI * freq) / sampFreq;
		const auto arg = omega*n;
		const auto sigValue = cosf(arg);

		const auto guiInvoerWaarde = static_cast<Int16>(signaalAmplitude*sigValue);

		GRAPH_DATA_YT_AddValue(invoerDataHandle, guiInvoerWaarde);

		/* filter data */
		const auto dacAmplitude = SchaalFaktor*2;
		const auto filterInvoer = static_cast<Int16>(dacAmplitude*sigValue);
		const auto filterwaarde = filter.filter(filterInvoer);
		const auto filterGUIWaarde = (grafiekGrootte*filterwaarde)/(4*dacAmplitude);
		const auto guiFilterWaarde = static_cast<Int16>(filterGUIWaarde);
		GRAPH_DATA_YT_AddValue(filterDataHandle, guiFilterWaarde);

		sprintf(tekstbuffer,"x[%d] = %d y[%d] = %d\n",n,filterInvoer,n,filterwaarde);
		SEGGER_RTT_WriteString(0,tekstbuffer);


		if (++n == sweepNumber)
		{
			n = 0;

			freq += sweepIncr;
			if (freq > endSweep)
				freq = beginSweep;
		}
		GUI_Exec();

	} while (true);

}

/***************************** Beste studenten, hieronder niets veranderen             **********************/
/***************************** Dear students, do noit change anytrhing below this line **********************/

STM32FilterApp::STM32FilterApp(): ESEBoardController(96),
                                   coeffsBuffer(AantalTaps,
                                                filterFixedCoeffs),
                                   filter(coeffsBuffer,filterInputBuffer,
                                          SchaalFaktor)
{
	/* Laat onderstaande statements zoals zij zijn /
	 * Leave following statements untouched. */
	schakelSysTick(Schakelaar::Uit);
	selekteerInput(STM32BoardController::DSBSignaalSelektor::BNCSignaal);
	selekteerADCAntiAlias(STM32BoardController::AntiAliasCtrl::AA5kHz);
	selekteerDACAntiAlias(STM32BoardController::AntiAliasCtrl::AA5kHz);

	__HAL_RCC_GPIOA_CLK_ENABLE();

	/* gebruik pin F11 om sampfrek te testen. */
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

 void STM32FilterApp::hoofdlus(const Schakelaar metSkoop)
{
	if (Schakelaar::Uit == (useScope = metSkoop))
	{
		GUI_Clear();

#ifdef InterfaceTaalNederlands
		static const char *melding1 = "Start van filter applikatie";
		static const char *melding2 = "Bekijk de externe oscilloscoop";
		SEGGER_RTT_WriteString(0, "Start van filter applikatie.\n");
		SEGGER_RTT_WriteString(0, "Gebruik Segger RTT om het filtergedrag te debuggen, indien nodig.\n");
#else
		static const char *melding1 = "Start of filter application.";
		static const char *melding2 = "Watch the ext oscilloscope";
		SEGGER_RTT_WriteString(0,"Start of the filter application.\n");
		SEGGER_RTT_WriteString(0,"Use Segger RTT to debug the filter performance, if required.\n");
#endif
		GUI_DispStringHCenterAt(melding1,120,80);
		GUI_DispStringHCenterAt(melding2,120,110);
		runFilter();
	}
	else
	{
		initGrafiekGUI(GUIMaxDataLengte);
#ifdef InterfaceTaalNederlands
		SEGGER_RTT_WriteString(0, "Start van filter test applikatie.\n");
		SEGGER_RTT_WriteString(0, "Gebruik Segger RTT om het filtergedrag te debuggen, indien nodig.\n");
#else
		SEGGER_RTT_WriteString(0,"Start of the filter test application.\n");
		SEGGER_RTT_WriteString(0,"Use Segger RTT to debug the filter performance, if required.\n");
#endif
		runGUITest();
	}
}


Schakelaar STM32FilterApp::geefBKUPGebruikGUISkoopKeuze()
{
	const auto stand = static_cast<Schakelaar>(backupGeheugenLezen(GebruikLCDBKUPVeld));
	return(stand);
}

void STM32FilterApp::zetBKUPGebruikLCDKeuze(const Schakelaar stand)
{
	const auto s = static_cast<UInt32>(stand);
	backupGeheugenOpslaan(GebruikLCDBKUPVeld,s);
}
void STM32FilterApp::initGrafiekGUI(const UInt32 aantalMeetPunten)
{
	if (WM_HWIN_NULL != grafiek)
		WM_ShowWindow(grafiek);
	else
	{
		grafiek = GRAPH_CreateEx(0, 0, grafiekGrootte, grafiekGrootte,
		                         WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_GRAPH0);

		GRAPH_SetColor(grafiek,GUI_WHITE,GRAPH_CI_BK);
		GRAPH_SetColor(grafiek,GUI_GRAY,GRAPH_CI_BORDER);
		GRAPH_SetColor(grafiek,GUI_DARKGRAY,GRAPH_CI_GRID);

		GRAPH_SetGridVis(grafiek,1);

		invoerDataHandle = GRAPH_DATA_YT_Create(GUI_BLUE, GUIMaxDataLengte, guiInvoerData, aantalMeetPunten);
		filterDataHandle = GRAPH_DATA_YT_Create(GUI_DARKGREEN, GUIMaxDataLengte, guiProcesData, aantalMeetPunten);

		constexpr int offsets[] = { 60,180 };
		auto index = 0;
		for (auto &handle : { invoerDataHandle,filterDataHandle})
		{
			GRAPH_AttachData(grafiek, handle);
			const auto os = offsets[index++];
			GRAPH_DATA_YT_SetOffY(handle, os);
		}

		guiSchaal = GRAPH_SCALE_Create(20, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 20);
		static constexpr auto ySchaalFaktor = 1.0f; //5.0f/grafiekGrootte;
		GRAPH_SCALE_SetFactor(guiSchaal,ySchaalFaktor);
		GRAPH_AttachScale(grafiek, guiSchaal);
		GRAPH_SetBorder(grafiek,20,0,0,0);
	}
}

