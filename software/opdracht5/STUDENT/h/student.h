/*************** DSB practicum opdracht 5 *****************/
/* $Id: student.h 298 2022-02-24 14:43:35Z ewout $     */
/*************** Code API van de student *********************/

#ifndef DSBOPDRACHT5_STUDENT_H
#define DSBOPDRACHT5_STUDENT_H

/*************** VUL HIER NAAM en STUDENTNUMMER in ************/
/*************** en verwijder de preprocessor error aanduiding */
//#error  "(student.h) nog geen naam en studentnummer ingevuld ingevuld."    
/********  Naam :               ******/
/********  Studentnummer :      ******/

#include <firfilter.h>

#include <ESEBoardController.h>
#include "firfilterexport.h"
#include <GUI.h>
#include <WM.h>
#include <TEXT.h>
#include <GRAPH.h>

/******************************************************************************************************/
/* Geachte student : vul in deze klasse jullie code in */
/******************************************************************************************************/

class STM32FilterApp : public ESEBoardController
{
public:
    explicit STM32FilterApp();

    /*! @brief De hoofdlus van het programma. Hierin wordt het filter toegepast.
	   The main loop of the program. The filter is applied here. */
	void hoofdlus(const Schakelaar);

	Schakelaar geefBKUPGebruikGUISkoopKeuze();
	void zetBKUPGebruikLCDKeuze(const Schakelaar stand);

protected:

	[[noreturn]] void runFilter();

	[[noreturn]] void runGUITest();

private:

	StaticFilterRingBuffer<AantalTaps> filterInputBuffer;
	FilterRingBuffer coeffsBuffer;

	/*! @brief Dit is het filter dat door jullie geimplementeerd werd in opdracht 4.
	 * This is the filter that you implemented in assignment 4. */
	FilterFirInt16 filter;


	char tekstbuffer[100];
	static constexpr auto DSB_ADC_Channel=ADS131A02::Kanaal::K2;
	static constexpr auto DSB_DAC_Channel=MAX5136::Kanaal::K2;

	Switch useScope = Schakelaar::Uit;

	/*! init de grafiek.
 	* @param het aantal meetpunten. */
	void initGrafiekGUI(const UInt32 );

	WM_HWIN grafiek = WM_HWIN_NULL;
	GRAPH_DATA_Handle invoerDataHandle,filterDataHandle;
	GRAPH_SCALE_Handle guiSchaal;

	static constexpr auto grafiekGrootte = 240;
	static constexpr auto signaalAmplitude = grafiekGrootte/4;

	static constexpr auto GUIMaxDataLengte=2*grafiekGrootte;
	Int16 guiInvoerData[GUIMaxDataLengte];
	Int16 guiProcesData[GUIMaxDataLengte];



	static constexpr auto GebruikLCDBKUPVeld = 3;


};





#endif /* DSB_STUDENT_H */

