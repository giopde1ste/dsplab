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
#include <complex>

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

/*! @brief Geef de grootte (lineair) van het polaire getal.
 * @return de grootte. */
float PolairGetal::Mag() const {
	return grootte;
}

/*! @brief Geef de fasehoek (0 .. 2*pi) van het polaire getal.
 * @return de fasehoek. */
float PolairGetal::Arg() const {
	return fase;
}

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

/*! @brief Constructors for the Complex class.
 * @note Geen template ==> maak deze constructors aan in de cpp file. */
Complex::Complex(const PolairGetal& polairGetal) {
	auto r = polairGetal.Mag();
	auto theta = polairGetal.Arg();
	x = r * cosf(theta);
	y = r * sinf(theta);
}

/*! @brief Importeer een polair getal. */
Complex& Complex::operator = (const PolairGetal& polairGetal) {
	auto r = polairGetal.Mag();
	auto theta = polairGetal.Arg();
	x = r * cosf(theta);
	y = r * sinf(theta);
	return *this;
}

bool Complex::operator == (const Complex& rhs) const {
	const bool antwoord = (x == rhs.x) && (y == rhs.y);
	return antwoord;
}

Complex  Complex::operator + (const Complex& rhs) const /* + overload */ {
	const Complex uit(x + rhs.x, y + rhs.y);
	return uit;
}

Complex  Complex::operator - (const Complex& rhs) const  /* - overload */ {
	const Complex uit(x - rhs.x, y - rhs.y);
	return uit;
}

Complex  Complex::operator * (const Complex& rhs) const  /* * overload */ {
	const Complex uit(x * rhs.x - y * rhs.y, x * rhs.y + y * rhs.x);
	return uit;
}
Complex  Complex::operator * (const float rhs) const  /* * met float overload */ {
	const Complex uit(x * rhs, y * rhs);
	return uit;
}
Complex  Complex::operator / (const Complex& rhs) const  /* / overload */ {
	const auto divider = rhs.x * rhs.x + rhs.y * rhs.y;
	const auto real = (x * rhs.x + y * rhs.y) / divider;
	const auto complex = (y * rhs.x - x * rhs.y) / divider;
	const Complex uit(real, complex);
	return uit;
}
Complex& Complex::operator += (const Complex& rhs)  /* += overload */ {
	x += rhs.x;
	y += rhs.y;
	return *this;
}
Complex& Complex::operator -= (const Complex& rhs) /* -= overload */ {
	x -= rhs.x;
	y -= rhs.y;
	return *this;
}

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

/*! @brief Conversie funkties naar grootte/argument.
	 * @note maak deze in de cpp file aan. */
float Complex::Mag() const  /* grootte (lineair) */ {
	const auto pol(PolairGetal(x,y));
	return pol.Mag();
}
float Complex::Arg() const  /* fasehoek (0 .. 2*pi) */ {
	const auto pol(PolairGetal(x,y));
	return pol.Arg();
}

/*! @brief Conversie naar polaire representatie.
 * @note maak deze aan */
PolairGetal Complex::polair() const {
	const auto pol(PolairGetal(*this));
	return pol;
}

/*! @brief De funktie levert de geconjugeerde versie van het huidige object. Maak aan. */
Complex Complex::conj() const {
	const auto com(Complex(x, -y));
	return com;
}
