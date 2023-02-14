/*!
@file

Opdracht 1 DSB practicum. Werk deze opdracht verder uit aan de hand van het kommentaar.
Assignment 1 DSB practical. Elaborate this assignment on the basis of the comments.

@version $Rev: 313 $
@author $Author: ewout $
@note  Werk deze code verder uit volgens de opdracht  / Complete this code according to the assignment.
@copyright Copyright 2006-2022 ir drs E.J Boks Hogeschool van Arnhem en Nijmegen
$Id: dsbComplex.cpp 313 2023-01-30 13:54:35Z ewout $
*/

#ifndef OnderwijsOntwikkeling
#if defined(InterfaceTaalNederlands)
#elif defined(InterfaceTaalEnglish)
#endif
#endif

/********  Naam/name     : Thomas van den Oever  ******/
/********  Studentnummer : 585101                ******/

#include <cmath>
#include <cassert>

#include <dsbComplex.h>

#ifdef BouwDesktopApp
#include <iostream>
#include <wx/string.h>
#include <wx/log.h>

#include <wx/arrimpl.cpp> // This is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(ComplexArray);

using namespace std;
#endif

#if defined(InterfaceTaalNederlands)
PolairGetal::PolairGetal(const Complex &z) : grootte(z.Mag()), fase(z.Arg())
{
}

#error “Dit deel van de software ontbreekt — this part of the software is missing.”
/* Beste leerling, dit deel van de software ontbreekt. Vul dit deel aan volgens de opdracht.
   Dear student, this part of the software is missing. Complete this part accoording to the assignment.
*/

#elif defined(InterfaceTaalEnglish)

PolarNumber::PolarNumber(const Complex &z) : magnitude(z.Mag()),
											 phase(z.Arg())
{
}

#error “Dit deel van de software ontbreekt — this part of the software is missing.”
/* Beste leerling, dit deel van de software ontbreekt. Vul dit deel aan volgens de opdracht.
   Dear student, this part of the software is missing. Complete this part accoording to the assignment.
*/

#endif /* Nederlands/Engels */

Complex Complex::sqrt() const
{
	/* In practice, square roots of complex numbers are more easily found by first converting
	 * to polar form and then using DeMoivre’s Theorem.
	 * Any complex numbera+bican bewritten asr(cosθ+isinθ)wherer=√a2+b2,cosθ=ar,
	 * and    sinθ=br(4)DeMoivre’s Theorem states that ifnis any positive real number,
	 * then(a+bi)n=rn(cosnθ+isinnθ).In particular, ifn=1/2, we have√a+bi=√r(cosθ2+isinθ2).
	 * */
#if defined(InterfaceTaalNederlands)
	const auto pg(polair());
#else
	const auto pg(polar());
#endif

	const auto ws = sqrtf(pg.Mag());
	const auto wa = pg.Arg() / 2.0f;
	const auto a = ws * cosf(wa);
	const auto b = ws * sinf(wa);
	const Complex wortelgetal(a, b);
	return (wortelgetal);
}
