/*!
@file
API van de c++ schil voor gebruik van de Cortex M processor.
@version $Rev: 2923 $
@author $Author: ewout $

@copyright Copyright 2016-2018 Hogeschool van Arnhem en Nijmegen, ir drs E.J Boks

$URL: http://svn.hanese.nl/docenten/svn/ewout/vakken/algemeen/software/STM32/STM32Basis/HAL/gemeenschappelijk/Libraries/BSP/STM32412G-Discovery/h/STM32412gDiscovery.h $
$Id: STM32412gDiscovery.h 2923 2018-03-20 14:49:52Z ewout $
************************************************************************/

#ifndef HANESECortexM_H
#define HANESECortexM_H

#include "algdef.h"

#ifdef FreeRTOSGebruikt
#include <semaphore.hpp>
using namespace cpp_freertos;
#endif

class CortexMProcessor
{
public:
	CortexMProcessor();

	static void schakelSysTick(const Schakelaar );

#ifdef ARMCORTEXM4
    /*! @brief Maak gebruik van de assembly sqrt implementatie op de Cortex-M4F */
    static inline WFT squareroot(const float input)
    {
        /* Using "w" constraint allows to use single precision VFP floating point registers.
         * But this does not work for double precision.
        */
       float result;
        asm volatile("vsqrt.f32 %0,%1" : "=w"(result) : "w"(input)); return(result);
    };
#endif

private:

	enum class FixedException
	{
		MemManage=16,
		Bus,
		Usage
	};

	static void schakelCoreException(const FixedException,const Schakelaar );

	/*! @brief Configureer de Usage Fault exception :
	 * @param Vang een deling door 0 op (true)  of niet [vervang door 0]
	 * @param Van een niet opgelijnde mem access op (true) of niet (false) */
	static void usageFaultInstelling(const bool,const bool);
};

#endif /* stm32Controller_h */
