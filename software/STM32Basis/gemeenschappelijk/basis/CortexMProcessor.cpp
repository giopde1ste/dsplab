/*!
@file
Implementatie van de c++ schil voor gebruik van de Cortex M Processor.
@version $Rev: 2923 $
@author $Author: ewout $

@copyright Copyright 2017-2018 Hogeschool van Arnhem en Nijmegen, ir drs E.J Boks

$URL: http://svn.hanese.nl/docenten/svn/ewout/vakken/algemeen/software/STM32/STM32Basis/HAL/gemeenschappelijk/Libraries/BSP/STM32412G-Discovery/STM32412gDiscovery.cpp $
$Id: STM32412gDiscovery.cpp 2923 2018-03-20 14:49:52Z ewout $
************************************************************************/

#include <CortexMProcessor.h>

#ifdef ARMCORTEXM3
#include <stm32f10x.h>
#elif defined(ARMCORTEXM0)
#include <stm32f0xx.h>
#elif defined(ARMCORTEXM4)

#if (defined(STM32F40x) || defined(STM32F41x))
#include <stm32f4xx.h>
#elif (defined(STM32L4) || defined(STM32L4Plus))
#include <stm32l4xx.h>
#elif defined(STM32F30X)
#include <stm32f3xx.h>
#else
#error "(CortexMProcessor) : niet gevonden."
#endif

#endif

CortexMProcessor::CortexMProcessor()
{
	schakelCoreException(FixedException::Bus,Schakelaar::Aan);
	schakelCoreException(FixedException::Usage,Schakelaar::Aan);
	usageFaultInstelling(true,false);  /* 02.12.2021 uitgezet omdat Segger EmWin crasht. */
}

void CortexMProcessor::schakelSysTick(const Schakelaar schakelaar)
{
	EnterBit(SysTick->CTRL,SysTick_CTRL_ENABLE_Pos,schakelaar);
}

void CortexMProcessor::schakelCoreException(const CortexMProcessor::FixedException fe,const Schakelaar stand)
{
	const auto bitnummer = static_cast<UInt32>(fe);
	EnterBit(SCB->SHCSR,bitnummer,stand);
}

void CortexMProcessor::usageFaultInstelling(const bool deelDoorNul, const bool nietOpgelijnd)
{
	auto &ccr = SCB->CCR;
	EnterBit(ccr,SCB_CCR_DIV_0_TRP_Pos,deelDoorNul);
	EnterBit(ccr,SCB_CCR_UNALIGN_TRP_Pos,nietOpgelijnd);
}

