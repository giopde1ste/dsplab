/************ Opdracht 4 (Filterbouw) voor ESE DSB practicum                ********/
/************ Werk deze code verder uit volgens de opdracht                 ********/
/************ Copyright 2006-2019 Hogeschool van Arnhem en Nijmegen         ********/
/************ ir drs E.J Boks                                               ********/

/* $Id: firfilter.h 298 2022-02-24 14:43:35Z ewout $                               */


#ifndef HANESE_FILTER_OPDRACHT4_H
#define HANESE_FILTER_OPDRACHT4_H

#include <assert.h>
#include <algdef.h>

#include <dsbRingBuffer.h>

/* synoniem voor de Int16 buffer */
using FilterRingBuffer = RingBuffer<Int16>;

template <const unsigned short Grootte>

using StaticFilterRingBuffer = StaticRingBuffer<Int16,Grootte>;

class FilterFirInt16
{
public:

#ifdef BouwDesktopApp
    /*! @brief Constructor van het filter met DYNAMISCHE geheugen allocatie/
     *         Constructor of the filter with DYNAMIC memory allocation.
     *         This can only be used on the desktop.
     * @param een ptr naar array van coefficienten. / a pointer to the C-array that holds the filter coefficients.
     * @param De schaalfaktor waarmee het MAC resultaat wordt teruggebracht na accumulatie /
     * The scale factor that must be used to reduce the MAC result after accumulation.
     * @note this function is inted for usage in desktop systems.*/
    FilterFirInt16(const FilterRingBuffer &,
                   const Int16);
#endif
	/*! @brief Constructor van het filter met STATISCHE geheugen allocatie/
	 *         Constructor of the filter with STATIC memory allocation.
     * @param De buffer voor de coefficienten/ A buffer containing the coefficients.
     * @param De buffer voor het vasthouden van de inputs/ Buffer for holding all input values.
     * @param De schaalfaktor waarmee het MAC resultaat wordt teruggebracht na accumulatie /
     * the scale factor that must be used to reduce the MAC result after accumulation.
     * @note this function is intended for usage in embedded systems. */
    FilterFirInt16(const FilterRingBuffer &,
                   const FilterRingBuffer &,
                   const Int16);


    /*! @brief A reset should bring the filter into the default state (no data in memory) */
    void reset();

    /*! @brief This is the main filter function. This function should implement the filter action
     * with the resident coefficients, the current sample and the buffer of old samples.
     * @param The current sample.
     * @return The filtered value.
     */
    Int16 filter(const Int16 );

private:
	
    FilterRingBuffer filterCoeffs;
    FilterRingBuffer filterMemory;
    
    UInt32 loopUnroll4;
    UInt32 unrollRem;

	/* De schaalfaktor (afhankelijk van het aantal gebruikte bits) /
	 * the scale factor (dependant on the number of allocated bits) */
    const Int16 scaleFactor;
};

#endif // HANESE_FILTER_OPDRACHT4_H
