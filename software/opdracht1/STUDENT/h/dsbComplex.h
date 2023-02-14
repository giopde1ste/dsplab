/*!
  @file 

  Opdracht 1 DSB practicum. Werk deze opdracht verder uit aan de hand van het kommentaar.
  Assignment 1 DSB practical. Elaborate this assignment on the basis of the comments.

  @version $Rev: 302 $
  @author $Author: ewout $ 
  @note  Werk deze code verder uit volgens de opdracht   / Complete this code according to the assignment.
  @copyright Copyright 2006-2022 ir drs E.J Boks Hogeschool van Arnhem en Nijmegen
  $Id: dsbComplex.h 302 2022-03-09 15:14:26Z ewout $
*/

#ifndef DSBCOMPLEX_H
#define DSBCOMPLEX_H

#include <algdef.h>
#include <string.h>
#include <math.h>

#include <DSBConfig.h>

#ifdef BouwDesktopApp
#include <wx/string.h>
#endif

#ifdef InterfaceTaalNederlands

/* voorwaarts */
class Complex;

/*! @brief een polair getal heeft een amplitude en richting als bepalende kenmerken. */
class PolairGetal
{
public:
    PolairGetal() = default;

    /*! @brief Std constructor met grootte en fasehoek. */
    explicit constexpr PolairGetal(const float amp,
                                   const float hoek) : grootte(amp),fase(hoek)
    {

    };

    /*! @brief Constructor op basis van een cartesisch complex getal. */
    explicit PolairGetal(const Complex &);

    /*! @brief Geef de grootte (lineair) van het polaire getal.
     * @return de grootte. */
    float Mag() const;
    /*! @brief Geef de fasehoek (0 .. 2*pi) van het polaire getal.
     * @return de fasehoek. */
    float Arg() const;

private:
    float grootte=0.0f,fase=0.0f;
};

/*! @class Een cartesisch complex getal bestaand uit een reëel en een imaginair gedeelte. */
class Complex
{
public:
    /*! @brief constexpr constructors for the Complex class. */
    explicit constexpr Complex(const float xin=0,const float yin=0) : x(xin),y(yin)
    {

    };

    /*! @brief Constructors for the Complex class.
     * @note Geen template ==> maak deze constructors aan in de cpp file. */
    explicit Complex(const PolairGetal &);

    /*! @brief Importeer een polair getal. */
    Complex & operator = (const PolairGetal &);

    /*! @brief Belangrijke overloaded operators van een complex getal.
     * @note maak deze funkties in de cpp file. */
    bool operator == (const Complex &rhs) const;
    Complex  operator + (const Complex &rhs) const; /* + overload */
    Complex  operator - (const Complex &rhs) const;  /* - overload */
    Complex  operator * (const Complex &rhs) const;  /* * overload */
    Complex  operator * (const float rhs) const;  /* * met float overload */
    Complex  operator / (const Complex &rhs) const;  /* / overload */
    Complex & operator += (const Complex &rhs);  /* += overload */
    Complex & operator -= (const Complex &rhs); /* -= overload */

    /*! @bereken de wortel van een complex getal.
        @note LET OP dit is niet noodzakelijk voor het practicum 1-5. */
    Complex sqrt() const;

    float Re() const
    {
        return(x);
    };

    float Im() const
    {
        return(y);
    };

    bool isImaginair() const
    {
        const bool isImagGetal = (fabsf(x) < 1.0e-7f);
        return(isImagGetal);
    }

    bool isReeel() const
    {
        const bool isReelGetal = (fabsf(y) < 1.0e-7f);
        return(isReelGetal);
    }

    /*! @brief Conversie funkties naar grootte/argument.
     * @note maak deze in de cpp file aan. */
    float Mag() const;      /* grootte (lineair) */
    float Arg() const;      /* fasehoek (0 .. 2*pi) */

    /*! @brief Conversie naar polaire representatie.
     * @note maak deze aan */
    PolairGetal polair() const;

    /*! @brief De funktie levert de geconjugeerde versie van het huidige object. Maak aan. */
    Complex conj() const;

protected:

private:
    friend class ComplexPrinter;

    float x,y;   /* het complexe getal z=x+jy */
};

#elif defined (InterfaceTaalEnglish)

/* forward declaration.  */
class Complex;

/*! @class a polar number has an amplitude and direction as defining characteristics. */
class PolarNumber
{
public:

    PolarNumber() = default;
    
	/*! @brief Std constructor with size and phase angle. */
	explicit constexpr PolarNumber(const float amp,
	                     const float angle) : magnitude(amp),phase(angle)
    {

    };

	/*! @brief Constructor based on a Cartesian complex number. */
	explicit PolarNumber(const Complex &);

	/*! @brief Specify the size (linear) of the polar number.
	 * @return The size. */
	float Mag() const;
	/*! @brief Enter the phase angle (0 .. 2 * pi) of the polar number.
	 * @return the phase angle. */
	float Arg() const;

private:
	float magnitude=0.0f,phase=0.0f;
};

/*! @class A Cartesian complex number consisting of a real and an imaginary part. */
class Complex
{
public:

	Complex() = default;

	/*! @brief Constructors for the Complex class.
	 * @note No template ==> create these constructors in the cpp file. */
	explicit constexpr Complex(const float xin,const float yin) : x(xin),y(yin)
	{

	}

	explicit Complex(const PolarNumber &);

	/*! @brief Import a polar number using the operator. */
	Complex& operator = (const PolarNumber &);

	/*! @brief Important overloaded operators of a complex number.
	 * @note make these functions in the cpp file. */
	bool operator == (const Complex &rhs) const;
	Complex  operator + (const Complex &rhs) const; /* + overload */
	Complex  operator - (const Complex &rhs) const;  /* - overload */
	Complex  operator * (const Complex &rhs) const;  /* * overload */
    Complex  operator * (const float rhs) const;  /* * (float) overload */
	Complex  operator / (const Complex &rhs) const;  /* / overload */
	Complex & operator += (const Complex &rhs);  /* += overload */
	Complex & operator -= (const Complex &rhs); /* -= overload */

	float Re() const
	{
		return(x);
	};

	float Im() const
	{
		return(y);
	};

	/*! @brief Conversion functions to size / argument.
	 * @note make these functions in the cpp file. */
	float Mag() const;      /* magnitude (linear) */
	float Arg() const;      /* phase angle (0 .. 2*pi) */

	/*! @brief Conversion to polar representation.
	 * @note make this function in the cpp file */
	PolarNumber polar() const;

	/*! @brief The function provides the conjugate version of the current object. */
	Complex conj() const;

	/*! @brief Calculate the square root of a complex number.
		@note This function is not required for laboratory assignments 1-5. */
	Complex sqrt() const;

protected:

private:
    friend class ComplexPrinter;

	float x=0.0f,y=0.0f;   /* The complexe number z=x+jy */
};

#else
#error "Defineer een taal !!!!\n Define a language !!!!"
#endif /* #if (InterfaceTaalNederlands) */

/*! @brief Uitdrukking voor het aanmaken van een kamfilter. 
    @note Studenten hoeven hier niets aan te doen / Student do not need to alter or use this class. */
class KamUitdrukking
{
public:
    KamUitdrukking() = default;

    enum class Type
    {
        Nulpunt,
        Pool
    };

    Type type = Type::Nulpunt;
    double straal = 1.0f;
    double halveBoogLengteGedeelte=1.0f;
    unsigned int aantal = 5;
    bool kompensatie = true;
};

#ifdef BouwDesktopApp

#include <wx/dynarray.h>
/*! @brief Een ComplexArray is een wxArray van Complex.
 */
WX_DECLARE_OBJARRAY(Complex, ComplexArray);

#endif

#endif /* DSBCOMPLEX_H */


