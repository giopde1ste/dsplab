/*!
@file
Opdracht 4+6 DSB practicum (applikatie API gedeelte).
 De student hoeft dit stuk NIET uit te werken / NO work is required from the student in this file/
@version $Rev: 298 $
@author $Author: ewout $

@copyright Copyright 2006-2020 ir drs E.J Boks, Hogeschool van Arnhem en Nijmegen. https://ese.han.nl

$URL: https://ese.han.nl/svn/dsbpracticum/trunk/2022/software/opdracht4/STUDENT/ontwerper/h/filterDesignerBasis.h $
$Id: filterDesignerBasis.h 298 2022-02-24 14:43:35Z ewout $
************************************************************************/

#ifndef HANESE_FILTERONTWERPER_Basis_H
#define HANESE_FILTERONTWERPER_Basis_H

#include <grafiekVenster.h>

#if defined (InterfaceTaalEnglish)
//using GrafiekVenster = GraphicsWindow;
#endif

using Frequentie = int;
using Frequency = Frequentie;

/*! @brief Deze klasse implementeert een wxWindow waarin de FIR berekening wordt uitgevoerd */
class FilterDesignerBasis
{

protected:

	FilterDesignerBasis() = default;

	/****** hulpfunkties / auxiliary functions *********/

	/*! @brief Bereken het aantal punten in het frequentie gebied gebaseerd op het aantal filtertaps /
	 * Calculate the number of points in the frequency range based on the number of filter taps.
	 * @param taps Het aantal taps/the number of taps.
	 * @return Het aantal punten in het frequentiespectrum / The number of points in the frequency spectrum.
	 */
	static unsigned int FreqSpectrumPunten(const unsigned int taps);

	/*! @brief Konverteer een variabeletype naar het aantal signed bits dat er voor staat.
	 * Convert a variable type to the number of signed bits before it.
	 * @param de grootte van het variabele typ in aantal bytes / the size of the variable type in number of bytes.
	 * @return de grootte in bits, rekening houdend met een sign bit /
	 * the size in bits, taking into account a sign bit. */
	static constexpr auto MaximumBereikSignedInt(const unsigned int varbreedte);

	/*! @brief Transformeer lineair --> decibel (dB) */
	static double compute_dB(const double);

	/* @brief Transformeer decibel (dB) --> lineair  */
	static double compute_Linear(const double );

	/****** Einde hulpfunkties / End of auxiliary functions *********/

	bool beginPuntBepaald=true, eindPuntBepaald=true;
	bool berekeningKlaar=false,testSituatie=false;
	bool testOrig,testSpline,toonCoeffs;

	Frequency sampFreq=0, testFreq=0;
	Frequency filterBegin=0, filterEind=0;
	Int16 testAmplitude=0;

	UInt16 taps=3,orde=1;
	Teller testIndex=0,testTekenIndex=0;
	PuntLijst filterSignaalLijst;

	/* Predefined pens for the axes (assen), time(tijd), filtered (gefilterd) and others. */
	const wxPen axisPen = wxPen(wxColour(wxT("BLACK")), 1, wxPENSTYLE_SOLID);
	const wxPen dikkeTimeDomainPen = wxPen(wxColour(wxT("RED")), 4, wxPENSTYLE_SOLID);
	const wxPen timeDomainPen = wxPen(wxColour(wxT("RED")), 2, wxPENSTYLE_SOLID);
	const wxPen freqDomainPen = wxPen(wxColour(wxT("BLUE")), 2, wxPENSTYLE_SOLID);
	const wxPen originalSignalPen = wxPen(wxColour(wxT("GREY")), 4, wxPENSTYLE_SOLID);
	const wxPen filterdSignalPen = wxPen(wxColour(wxT("ORANGE")), 4, wxPENSTYLE_SOLID);

	/* static constexpr auto TijdGrafiekBreedte = 500;
	static constexpr auto TijdGrafiekHoogte= 200;
	static constexpr auto FreqGrafiekBreedte= TijdGrafiekBreedte;
	static constexpr auto FreqGrafiekHoogte= TijdGrafiekHoogte;
	static constexpr auto TestGrafiekBreedte= 840;
	static constexpr auto TestGrafiekHoogte= 200;
	*/
};




#endif // GEMWAARDE_H
