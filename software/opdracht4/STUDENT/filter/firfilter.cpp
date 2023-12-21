/*!
@file

Opdracht 4 DSB practicum (filter gedeelte). Werk deze opdracht verder uit aan de hand van het kommentaar.
Assignment 4 DSB practical (filter part). Elaborate this assignment on the basis of the comments.

@version $Rev: 313 $
@author $Author: ewout $

@copyright Copyright 2006-2020 ir drs E.J Boks, Hogeschool van Arnhem en Nijmegen. https://ese.han.nl

$URL: https://ese.han.nl/svn/dsbpracticum/trunk/2022/software/opdracht4/STUDENT/filter/firfilter.cpp $
$Id: firfilter.cpp 313 2023-01-30 13:54:35Z ewout $
************************************************************************/

/* $Id: firfilter.cpp 313 2023-01-30 13:54:35Z ewout $                               */

#include <assert.h>
#include <math.h>

#include "firfilter.h"

#if (InterfaceTaalNederlands)
#ifdef BouwDesktopApp
FilterFirInt16::FilterFirInt16(const FilterRingBuffer &coeffs,
                               const Int16 schaal) :  filterCoeffs(coeffs),
                                                      filterMemory(coeffs.geefAantal()),
                                                      loopUnroll4(coeffs.geefAantal()/4),
                                                      unrollRem(coeffs.geefAantal() % 4),
                                                      scaleFactor(schaal)
{

}
#endif

FilterFirInt16::FilterFirInt16(const FilterRingBuffer &coeffs,
                               const FilterRingBuffer &werkgeheugen,
                               const Int16 schaal) :  filterCoeffs(coeffs),
                                                      filterMemory(werkgeheugen),
                                                      loopUnroll4(coeffs.geefAantal()/4),
                                                      unrollRem(coeffs.geefAantal() % 4),
                                                      scaleFactor(schaal)
{

}

#elif defined (InterfaceTaalEnglish)

#ifdef BouwDesktopApp
FilterFirInt16::FilterFirInt16(const FilterRingBuffer &coeffs,
                               const Int16 scale) :  filterCoeffs(coeffs),
                                                      filterMemory(coeffs.giveSize()),
                                                      loopUnroll4(coeffs.giveSize()/4),
                                                      unrollRem(coeffs.giveSize() % 4),
                                                      scaleFactor(scale)
{

}
#endif

FilterFirInt16::FilterFirInt16(const FilterRingBuffer &coeffs,
                               const FilterRingBuffer &workMemory,
                               const Int16 scale) :  filterCoeffs(coeffs),
                                                      filterMemory(workMemory),
                                                      loopUnroll4(coeffs.giveSize()/4),
                                                      unrollRem(coeffs.giveSize() % 4),
                                                      scaleFactor(scale)
{

}


#endif

void FilterFirInt16::reset()
{

//#error “Dit deel van de software ontbreekt — this part of the software is missing.”
/* Beste leerling, dit deel van de software ontbreekt. Vul dit deel aan volgens de opdracht.  
   Dear student, this part of the software is missing. Complete this part accoording to the assignment.
*/
    filterMemory.reset();

}


/* Implementatie van het filter */
Int16 FilterFirInt16::filter(const Int16 input)
{
/* Beste leerling, dit deel van de software ontbreekt. Vul dit deel aan volgens de opdracht.  
   Dear student, this part of the software is missing. Complete this part accoording to the assignment.
*/

    // Discreet convolution!
    float out = 0.0f;
    filterMemory.schrijf(input);

    for (int i = 0; i < filterCoeffs.geefAantal(); i++)
    {
        auto temp = filterCoeffs[i];
        out += (filterCoeffs[i] / static_cast<float>(scaleFactor)) * filterMemory.lees();
        //filterMemory.schrijf(out);
    }

    return out;
}

//for (int i = 0; i < filterCoeffs.geefAantal(); i++)
//{
//    float coeff = filterCoeffs[i] / static_cast<float>(scaleFactor);
//    auto nvalue = (input * coeff) * filterMemory.lees(-1);
//    filterMemory.schrijf(nvalue);
//}
//out = filterMemory.lees(-1);
//filterMemory.schrijf(0);

//float FIRFilter_calc(FIRFilter* fir, float inputVal) {
//
//    float out = 0.0f;
//
//    /*Implementing CIRCULAR BUFER*/
//    //Store the latest sample=inputVal into the circular buffer
//    fir->buff[fir->buffIndex] = inputVal;
//
//    //Increase the buffer index. retrun to zero if it reach the end of the index (circular buffer)
//    fir->buffIndex++;
//    uint8_t sumIndex = fir->buffIndex;
//    if (fir->buffIndex == FIR_FILTER_LENGTH) {
//        fir->buffIndex = 0;
//    }
//
//    //Compute the filtered sample with convolution
//    fir->out = 0.0f;
//    for (int i = 0;i < FIR_FILTER_LENGTH;i++) {
//        //decrese sum index and warp it if necessary
//        if (sumIndex > 0) {
//            sumIndex--;
//        }
//        else {
//            sumIndex = FIR_FILTER_LENGTH - 1;
//        }
//        //The convolution process: Multyply the impulse response with the SHIFTED input sample and add it to the output
//        fir->out = fir->out + fir->buff[sumIndex] * FIR_FILTER_IMPULSE_RESPONSE[i];
//    }
//
//    //return the filtered data
//    return out;
//
//}

