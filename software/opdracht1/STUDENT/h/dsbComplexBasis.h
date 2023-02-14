/*!
  @file 

  Opdracht 1 DSB practicum. Werk deze opdracht verder uit aan de hand van het kommentaar.
  Assignment 1 DSB practical. Elaborate this assignment on the basis of the comments.

  @version $Rev: 313 $
  @author $Author: ewout $ 
  @note  Werk deze code verder uit volgens de opdracht   / Complete this code according to the assignment.
  @copyright Copyright 2006-2022 ir drs E.J Boks Hogeschool van Arnhem en Nijmegen
  $Id: dsbComplexBasis.h 313 2023-01-30 13:54:35Z ewout $
*/

#ifndef DSBCOMPLEXBasis_H
#define DSBCOMPLEXBasis_H

#include <algdef.h>
#include <string.h>
#include <math.h>

#include <DSBConfig.h>

#ifdef BouwDesktopApp

#include <wx/string.h>

/* voorwaarts */
#if defined (InterfaceTaalNederlands)
class PolairGetal;
#elif defined (InterfaceTaalEnglish)
class PolarNumber;
using PolairGetal = PolarNumber;
#endif

/*! @brief een polair getal heeft een amplitude en richting als bepalende kenmerken. */
class PolairPrinter
{
public:
	/*! @brief Std constructor met grootte en fasehoek. */
	explicit PolairPrinter(const PolairGetal &);

    enum class Formaat
    {
        Standaard,
        Importeerbaar
    };
    
    static wxString str(const PolairGetal &,
                        const Formaat formaat = Formaat::Standaard,
                        const Teller precisie=3);

	/*! @brief logging funktie */
	wxString str(const Teller precisie = 3) const;
	
	/*! @brief Formatteer als string in een formaat dat ook geimporteerd kan worden. */
	wxString wiskstr(const Teller precisie = 3) const;

private:
	const PolairGetal &p;
};

class Complex;

class ComplexPrinter
{
public:
	explicit ComplexPrinter(Complex &) ;

    enum class Formaat
    {
        Standaard,
        Importeerbaar
    };
    
	static wxString str(const Complex &,const Formaat formaat = Formaat::Standaard);

	/*! @brief print het complexe getal als wxString */
	wxString str() const;
	/*! @brief Formatteer als string in een formaat dat ook geimporteerd kan worden. */
	wxString wiskstr() const;

	/*! @brief importeer een string in het formaat a+bj of r,phi in deze variabele.
	 * @param De tekst met het compelxe getal.
	 * @note Het formaat moet zijn : a+bj of r,phi .
	 * phi is standaard een radiale hoek. Echter,
	 * phi mag daarbij worden voorafgegaan door een p, dan is de hoek een vermenigvuldiging met pi,
	 * of d dan is de hoek pih een gradenhoek.
	 * @return Is de importeer aktie gelukt of niet. */
	RetCode importeer(const wxString &);


private:
	Complex &z;
};

#endif

#endif


