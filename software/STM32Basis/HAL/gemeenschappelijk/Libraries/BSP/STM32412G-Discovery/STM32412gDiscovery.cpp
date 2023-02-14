/*!
@file
Implementatie van de c++ schil voor gebruik van de STM32412g Discovery kit.
@version $Rev: 4028 $
@author $Author: ewout $

@copyright Copyright 2017-2018 Hogeschool van Arnhem en Nijmegen, ir drs E.J Boks

$URL: http://svn.hanese.nl/docenten/svn/ewout/vakken/algemeen/software/STM32/STM32Basis/HAL/gemeenschappelijk/Libraries/BSP/STM32412G-Discovery/STM32412gDiscovery.cpp $
$Id: STM32412gDiscovery.cpp 4028 2020-11-27 16:11:57Z ewout $
************************************************************************/

#include <STM32412gDiscovery.h>
#include <algdef.h>

#include <assert.h>
#include <diskio.h>
#include "stm32412g_discovery_qspi.h"
#include "stm32412g_discovery_eeprom.h"


#ifndef HAL_RTC_MODULE_ENABLED
#error "HAL_RTC_MODULE_ENABLED moet aan staan voor dit BSP."
#endif

STM32412gDiscovery::STM32412gDiscovery(const UInt32 freq)
{
	
	/* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user
         can eventually implement his proper time base source (a general purpose
         timer for example or other time source), keeping in mind that Time base
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
         handled in milliseconds rtosBasisDSB.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
	/* Configure Flash prefetch, Instruction cache, Data cache */
#if (INSTRUCTION_CACHE_ENABLE != 0U)
	__HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
#endif /* INSTRUCTION_CACHE_ENABLE */

#if (DATA_CACHE_ENABLE != 0U)
	__HAL_FLASH_DATA_CACHE_ENABLE();
#endif /* DATA_CACHE_ENABLE */

#if (PREFETCH_ENABLE != 0U)
	__HAL_FLASH_PREFETCH_BUFFER_ENABLE();
#endif /* PREFETCH_ENABLE */
	
	/* Set Interrupt Group Priority */
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	
	/* Use TIM14 as time base source and configure 1ms tick (default clock after Reset is HSI) */
	HAL_InitTick(TICK_INT_PRIORITY);
	
	SYSCLKConfig_STOP(freq);
	
	/** Enables the Clock Security System */
	HAL_RCC_EnableCSS();
	
	enableSWO(8);  /* 24/8 = 3 MHz output */
	
	/* tijdens slaap en stop moet clk voor debug blijven draaien */
#ifndef NDEBUG
	HAL_DBGMCU_EnableDBGSleepMode();
	HAL_DBGMCU_EnableDBGStandbyMode();
#endif
	
	/* initialiseer de peripheral drivers */
	gpioInit();
	ledsInit();
	
	qspiInit();
// doet het niet --> uitzoeken sdioInit();
	rngInit();
	rtcInit();
}



TIM_HandleTypeDef STM32412gDiscovery::Tim13Handle;

void STM32412gDiscovery::knipperBlauwLED(const STM32412gDiscovery::KnipperLEDStand stand) const
{
	assert(stand<KnipperLEDStand::Laatste);
	
	switch(stand)
	{
		case KnipperLEDStand::Stop:
		{
			BSP_LED_Off(LED4);
			HAL_NVIC_DisableIRQ(TIM8_UP_TIM13_IRQn);
			HAL_TIM_Base_Stop_IT(&Tim13Handle);
		}
			break;
		case KnipperLEDStand::PermanentAan:
		{
			BSP_LED_On(LED4);
			HAL_NVIC_DisableIRQ(TIM8_UP_TIM13_IRQn);
			HAL_TIM_Base_Stop_IT(&Tim13Handle);
		}
			break;
		
		default:
		{
			static const UInt16 knipperStanden[] =
				{
					0,
					0,
					60000,   /* 96E6/(3200*60000) = 1/2 Hz */
					30000,   /* 1 Hz */
					15000,   /* 2 Hz */
					5000     /* 6 Hz */
				};
			
			auto &init = Tim13Handle.Init;
			init.Period = knipperStanden[static_cast<Teller>(stand)];
			init.Prescaler = 3200;
			init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
			init.CounterMode = TIM_COUNTERMODE_UP;
			if (HAL_TIM_Base_Init(&Tim13Handle) != HAL_OK)
			{
				/* Initialization Error */
				StopHier();
			} else
			{
				HAL_NVIC_SetPriority(TIM8_UP_TIM13_IRQn, 14, 0);
				
				HAL_TIM_Base_Start_IT(&Tim13Handle);
				HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);
			}
		}
	}
}

void STM32412gDiscovery::setLED(const LEDKleur kleur,
                                const Schakelaar stand) const
{
	assert(kleur < LEDKleur::LaatsteKleur);
	
	static const Led_TypeDef stLEDs[4] = { LED_GREEN,LED_ORANGE,LED_RED,LED_BLUE };
	
	/* Een echte lambda funktie !! */
	auto schakelFunk = [] (const LEDKleur lkleur,const Schakelaar s)
	{
		const auto k = static_cast<Teller>(lkleur);
		switch(s)
		{
			case Schakelaar::Uit:
				BSP_LED_Off(stLEDs[k]);
				break;
			case Schakelaar::Aan:
				BSP_LED_On(stLEDs[k]);
				break;
			case Schakelaar::Om:
				BSP_LED_Toggle(stLEDs[k]);
				break;
			default:
				StopHier();
				break;
		}
	};
	
	if (kleur < LEDKleur::AllColours)
		schakelFunk(kleur,stand);
	else
	{
		static const LEDKleur kleuren[] = { LEDKleur::Green,
		                                    LEDKleur::Orange,
		                                    LEDKleur::Red,
		                                    LEDKleur::Blue };
		
		for (auto k : kleuren)
			schakelFunk(k,stand);
		
	}
}

STM32412gDiscovery::KnopID STM32412gDiscovery::wachtOpEenKnop(const bool stopMode)
{
	resetKnop();
	
	wachtFunktie(stopMode);

#ifndef FreeRTOSGebruikt
	const auto k = geefKnopStand();
	return(k);
#else
	KnopID knop = KnopID ::Onbekend;
	
	for (Teller k=1;k<5;k++)
	{
		CountingSemaphore &sem = STM32412gDiscovery::knopSems[k];
		if (true == sem.Take(0))
		{
			knop = static_cast<STM32412gDiscovery::KnopID>(k);
			break;
		}
	}
	return(knop);
#endif
}


void STM32412gDiscovery::wachtOpKnop(const KnopID knopStand, const bool stopMode)
{
	resetKnop();
	
	bool magGaan=false;
	
	do
	{
		wachtFunktie(stopMode);

#ifndef FreeRTOSGebruikt
		const auto k = geefKnopStand();
		magGaan =  ((knopStand == k) ? true : false);
#else
		const auto index = static_cast<Teller>(knopStand);
		knopSems[index].Take();
		magGaan = true;
#endif
	} while (false == magGaan);
	
}

Schakelaar STM32412gDiscovery::geefKnopStand(const STM32412gDiscovery::KnopID knopStand) const
{
	/* knoppen op :
	 * centrum - pa0  --> EXTI0
	 * links   - pf15 --> EXTI15
	 * rechts  - pf14 --> EXTI14
	 * boven   - pg0  --> EXTI0
	 * onder   - pg1  --> EXTI1  */
	
	const JOYState_TypeDef js = BSP_JOY_GetState();
	const auto knopvert = static_cast<STM32412gDiscovery::KnopID>(js);
	
	return((knopStand == knopvert) ? Schakelaar::Aan : Schakelaar::Uit);
}

void STM32412gDiscovery::wachtFunktie(const bool stopMode)
{
	/* stop de systick en andere timer interrupts , deze maakt anders de wfi ongedaan */
	HAL_NVIC_DisableIRQ(TIM8_TRG_COM_TIM14_IRQn);
	schakelSysTick(Schakelaar::Uit);
	
	if (true == stopMode)
	{
		HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON,PWR_STOPENTRY_WFI);
		
		/* STM32 gaat verder na knop interrupt */
		SYSCLKConfig_STOP(96);  /* breng PLL en HSE weer tot leven */
	}
	else  /* wacht op interrupt */
	{
		slaapMode();
	}
	HAL_NVIC_EnableIRQ(TIM8_TRG_COM_TIM14_IRQn);
	schakelSysTick(Schakelaar::Aan);
}


TIM_HandleTypeDef STM32412gDiscovery::Tim12Handle;

void STM32412gDiscovery::wachtFunktie(const unsigned short msecs,
                                      const bool slaap) const
{
	__HAL_RCC_TIM12_CLK_ENABLE();
	Tim12Handle.Instance = TIM12;
	
	Tim12Handle.Init.Period        = 1000*msecs;
	Tim12Handle.Init.Prescaler     = 9600;
	Tim12Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	Tim12Handle.Init.CounterMode   = TIM_COUNTERMODE_UP;
	if(HAL_TIM_Base_Init(&Tim12Handle) != HAL_OK)
	{
		/* Initialization Error */
		StopHier();
	}
	else
	{
		HAL_NVIC_SetPriority(TIM8_BRK_TIM12_IRQn, 14, 0);
		
		HAL_TIM_Base_Start_IT(&Tim12Handle);
		HAL_NVIC_EnableIRQ(TIM8_BRK_TIM12_IRQn);
	}
}

bool STM32412gDiscovery::timer2Afgelopen=false;

void STM32412gDiscovery::timer2IsAfgelopen()
{
	timer2Afgelopen=true;
}

#ifndef FreeRTOSGebruikt
STM32412gDiscovery::KnopID STM32412gDiscovery::knop=KnopID::Onbekend;
#endif
void STM32412gDiscovery::resetKnop()
{
#ifndef FreeRTOSGebruikt
	knop=KnopID::Onbekend;
#endif
}

void STM32412gDiscovery::slaapMode() const
{
	/* This option is used to ensure that store operations are completed */
#if defined ( __CC_ARM   )
	__force_stores();
#endif
	/* Request Wait For Interrupt */
	asm volatile ("wfi");
	//__WFI();
}

/* ##################################    Debug Output  function  ############################################ */

#define ITM_ENA (*(volatile unsigned int*)0xE0000E00) // ITM Enable
#define ITM_TPR (*(volatile unsigned int*)0xE0000E40) // Trace Privilege Register
#define ITM_TCR (*(volatile unsigned int*)0xE0000E80) // ITM Trace Control Reg.
#define ITM_LSR (*(volatile unsigned int*)0xE0000FB0) // ITM Lock Status Register
#define DHCSR (*(volatile unsigned int*)0xE000EDF0) // Debug register
#define DEMCR (*(volatile unsigned int*)0xE000EDFC) // Debug register
#define TPIU_ACPR (*(volatile unsigned int*)0xE0040010) // Async Clock + prescaler register
#define TPIU_SPPR (*(volatile unsigned int*)0xE00400F0) // Selected Pin Protocol Register
#define DWT_CTRL (*(volatile unsigned int*)0xE0001000) // DWT Control Register
#define FFCR (*(volatile unsigned int*)0xE0040304) // Formatter and flush
// Control Register
//const UInt32 CortexM3Processor::_ITMPort = 0; // The stimulus port from which SWO data is received and displayed.
//const UInt32 CortexM3Processor::TargetDiv = 1;// Has to be calculated according to the CPU speed and the output baud rate
void STM32412gDiscovery::enableSWO(const UInt32 targetDiv,const UInt32 itmPort)
{

/* code is afkomstig van Segger J-link handleiding pdf,
 * bladzijde 74.
 */
	
	UInt32 StimulusRegs = ITM_ENA & (~(1 << itmPort));
//
// Enable access to SWO registers
//
	DEMCR |= (1 << 24);
	ITM_LSR = 0xC5ACCE55;
//
// Initially disable ITM and stimulus port
// To make sure that nothing is transferred via SWO
// when changing the SWO prescaler etc.
//
	ITM_ENA = StimulusRegs; // Disable ITM stimulus port
	ITM_TCR = 0; // Disable ITM
//
// Initialize SWO (prescaler, etc.)
//
	TPIU_SPPR = 0x00000002; // Select NRZ mode
	TPIU_ACPR = targetDiv - 1; // Example: 72/48 = 1,5 MHz
	ITM_TPR = 0x00000000;
	DWT_CTRL = 0x400003FE;
	FFCR = 0x00000100;
//
// Enable ITM and stimulus port
//
	ITM_TCR = 0x1000D; // Enable ITM
	ITM_ENA = StimulusRegs | (1 << itmPort); // Enable ITM stimulus port
	
}


UInt32 STM32412gDiscovery::ITMPrint(const UInt32 ch)
{
	return(::ITM_SendChar(ch));
}

void STM32412gDiscovery::ITMPrint(char const * const buffer)
{
	const char *bufPtr = buffer;
	char teken;
	
	do
	{
		ITMPrint(teken = *(bufPtr++));
	} while(teken != '\0') ;
}


/* wordt aangeroepen in de EXTI0 IRQ handler */
#ifndef FreeRTOSGebruikt
STM32412gDiscovery::KnopID STM32412gDiscovery::knopGedrukt(const KnopID id)
{
	return(knop = id);
}

STM32412gDiscovery::KnopID STM32412gDiscovery::geefKnopStand() const
{
	return(knop);
}
#endif

/**
 * @brief  Configures system clock after wake-up from STOP: enable HSE, PLL
 *         and select PLL as system clock source.
 * @param  None
 * @retval None
 */
void STM32412gDiscovery::SYSCLKConfig_STOP(const UInt32 freq)
{
	assert(freq <= 100);
	
	/* 8 MHz klok in uit ST-Link, 96 MHz SysClk */
	RCC_OscInitTypeDef RCC_OscInitStruct;
	
	/**Configure the main internal regulator output voltage
	*/
	__HAL_RCC_PWR_CLK_ENABLE();
	
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	
	/**Initializes the CPU, AHB and APB busses clocks
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_HSE
	                                   | RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 4;
	RCC_OscInitStruct.PLL.PLLN = freq;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	RCC_OscInitStruct.PLL.PLLR = 2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
		StopHier();
	else
	{
		/**Initializes the CPU, AHB and APB busses clocks
		*/
		RCC_ClkInitTypeDef RCC_ClkInitStruct;
		RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
		                              | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
		RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
		RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
		
		if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
			StopHier();
		else
		{
			RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
			
			PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC | RCC_PERIPHCLK_SDIO
			                                           | RCC_PERIPHCLK_CLK48;
			PeriphClkInitStruct.PLLI2S.PLLI2SN = 72;
			PeriphClkInitStruct.PLLI2S.PLLI2SM = 4;
			PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
			PeriphClkInitStruct.PLLI2S.PLLI2SQ = 3;
			PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
			PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLI2SQ;
			PeriphClkInitStruct.SdioClockSelection = RCC_SDIOCLKSOURCE_CLK48;
			PeriphClkInitStruct.PLLI2SSelection = RCC_PLLI2SCLKSOURCE_PLLSRC;
			if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
				StopHier();
			else
			{
				HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSE, RCC_MCODIV_1);
				
				/**Enables the Clock Security System
				*/
				HAL_RCC_EnableCSS();
				
				/**Configure the Systick interrupt time
				*/
				HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);
				
				/**Configure the Systick
				*/
				HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
				
				/* SysTick_IRQn interrupt configuration */
				HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
			}
		}
	}
}

void STM32412gDiscovery::initWaakhond(const UInt16 prescaler,
                                      const UInt16 venster)
{
	waakhond.Init.Prescaler = prescaler;
	waakhond.Init.Reload = venster;
	
	if (HAL_OK != HAL_IWDG_Init(&waakhond))
		StopHier();
	
}

void STM32412gDiscovery::aaiWaakhond()
{
	HAL_IWDG_Refresh(&waakhond);
}

void STM32412gDiscovery::coreReset() const
{
	NVIC_SystemReset();
}

FoutCode STM32412gDiscovery::backupGeheugenOpslaan(const UInt16 adres, const UInt32 waarde)
{
	assert(adres<20);
	auto retkode = FoutCode::Onbekend;
	
	if (false == (adres<20))
		retkode = FoutCode::Fout;
	else
	{
		const auto hwAdres = (RTC_BKP_DR0 + adres);
		HAL_RTCEx_BKUPWrite(&hrtc, hwAdres, waarde);
		retkode = FoutCode::Ok;
	}
	return(retkode);
}

UInt32 STM32412gDiscovery::backupGeheugenLezen(const UInt16 adres)
{
	assert(adres<20);
	
	UInt32 waarde = 0;
	
	
	if (true == (adres<20))
	{
		const auto hwAdres = (RTC_BKP_DR0 + adres);
		waarde = HAL_RTCEx_BKUPRead(&hrtc,hwAdres);
	}
	
	return(waarde);
}

UInt32 STM32412gDiscovery::revEndianess(const UInt32 value) const
{
	UInt32 result;
	__ASM volatile ("rev %0, %1" : "=r" (result) : "r" (value) );
	return(result);
}

void  STM32412gDiscovery::ledsInit()
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	/* -1- Enable GPIO Clock (to be able to program the configuration registers) */
	LEDx_GPIO_CLK_ENABLE();
	
	/* -2- Configure IO in output push-pull mode to drive external LEDs */
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	
	GPIO_InitStruct.Pin = LED1_PIN + LED2_PIN + LED3_PIN + LED4_PIN;
	HAL_GPIO_Init(LEDx_GPIO_PORT, &GPIO_InitStruct);
	
	BSP_LED_Off(LED1);
	BSP_LED_Off(LED2);
	BSP_LED_Off(LED3);
	BSP_LED_Off(LED4);
	
	__HAL_RCC_TIM13_CLK_ENABLE();
	Tim13Handle.Instance = TIM13;
}

void STM32412gDiscovery::eepromInit()
{
	BSP_EEPROM_Init();
}


#define QSPI_BK1_IO3_Pin GPIO_PIN_6
#define QSPI_BK1_IO2_Pin GPIO_PIN_7
#define QSPI_BK1_IO0_Pin GPIO_PIN_8
#define QSPI_BK1_IO1_Pin GPIO_PIN_9

#define QSPI_CLK_Pin GPIO_PIN_2
#define QSPI_CLK_GPIO_Port GPIOB
#define QSPI_BK1_NCS_Pin GPIO_PIN_6
#define QSPI_BK1_NCS_GPIO_Port GPIOG
#define uSD_D0_Pin GPIO_PIN_8
#define uSD_D1_Pin GPIO_PIN_9
#define uSD_D2_Pin GPIO_PIN_10
#define uSD_D3_Pin GPIO_PIN_11
#define uSD_CLK_Pin GPIO_PIN_12
#define uSD_CMD_Pin GPIO_PIN_2
#define uSD_CMD_GPIO_Port GPIOD


void STM32412gDiscovery::gpioInit()
{
	BSP_JOY_Init(JOY_MODE_EXTI);  /* BPS joystick init */
}

void STM32412gDiscovery::qspiInit()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/* Peripheral clock enable */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_QSPI_CLK_ENABLE();
	
	/**QUADSPI GPIO Configuration
	PF6     ------> QUADSPI_BK1_IO3
	PF7     ------> QUADSPI_BK1_IO2
	PF8     ------> QUADSPI_BK1_IO0
	PF9     ------> QUADSPI_BK1_IO1
	PB2     ------> QUADSPI_CLK
	PG6     ------> QUADSPI_BK1_NCS
	*/
	GPIO_InitStruct.Pin = QSPI_BK1_IO3_Pin|QSPI_BK1_IO2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF9_QSPI;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = QSPI_BK1_IO0_Pin|QSPI_BK1_IO1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = QSPI_CLK_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF9_QSPI;
	HAL_GPIO_Init(QSPI_CLK_GPIO_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = QSPI_BK1_NCS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
	HAL_GPIO_Init(QSPI_BK1_NCS_GPIO_Port, &GPIO_InitStruct);
	
	hqspi.Instance = QUADSPI;
	hqspi.Init.ClockPrescaler = 0;
	hqspi.Init.FifoThreshold = 4;
	hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
	hqspi.Init.FlashSize = 23;
	hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_1_CYCLE;
	hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0;
	hqspi.Init.FlashID = QSPI_FLASH_ID_1;
	hqspi.Init.DualFlash = QSPI_DUALFLASH_DISABLE;
	if (HAL_QSPI_Init(&hqspi) != HAL_OK)
		StopHier();
	else
	{
		/* flash verder nog uitwerken :
		 * dma conflict met usart6 dma2 stream 7 request */
		BSP_QSPI_Init();
	}
}

void STM32412gDiscovery::rngInit()
{
	/* Peripheral clock enable */
	__HAL_RCC_RNG_CLK_ENABLE();
	
	hrng.Instance = RNG;
	if (HAL_RNG_Init(&hrng) != HAL_OK)
		StopHier();
}

void STM32412gDiscovery::sdioInit()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/* Peripheral clock enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_SDIO_CLK_ENABLE();
	__HAL_RCC_DMA2_CLK_ENABLE();
	
	/**SDIO GPIO Configuration
	PC8     ------> SDIO_D0
	PC9     ------> SDIO_D1
	PC10     ------> SDIO_D2
	PC11     ------> SDIO_D3
	PC12     ------> SDIO_CK
	PD2     ------> SDIO_CMD
	*/
	GPIO_InitStruct.Pin = uSD_D0_Pin | uSD_D1_Pin | uSD_D2_Pin | uSD_D3_Pin
	                      | uSD_CLK_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = uSD_CMD_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
	HAL_GPIO_Init(uSD_CMD_GPIO_Port, &GPIO_InitStruct);
	
	/* Peripheral DMA start*/
	hdma_sdio_rx.Instance = DMA2_Stream3;
	hdma_sdio_rx.Init.Channel = DMA_CHANNEL_4;
	hdma_sdio_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma_sdio_rx.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_sdio_rx.Init.MemInc = DMA_MINC_ENABLE;
	hdma_sdio_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	hdma_sdio_rx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	hdma_sdio_rx.Init.Mode = DMA_PFCTRL;
	hdma_sdio_rx.Init.Priority = DMA_PRIORITY_MEDIUM;
	hdma_sdio_rx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
	hdma_sdio_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
	hdma_sdio_rx.Init.MemBurst = DMA_MBURST_INC4;
	hdma_sdio_rx.Init.PeriphBurst = DMA_PBURST_INC4;
	if (HAL_DMA_Init(&hdma_sdio_rx) != HAL_OK)
		StopHier();
	else
	{
		__HAL_LINKDMA(&hsd, hdmarx, hdma_sdio_rx);
		
		/* DMA2_Stream3_IRQn interrupt configuration */
		HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 10, 0);
		HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
		
		hdma_sdio_tx.Instance = DMA2_Stream6;
		hdma_sdio_tx.Init.Channel = DMA_CHANNEL_4;
		hdma_sdio_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma_sdio_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_sdio_tx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_sdio_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
		hdma_sdio_tx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
		hdma_sdio_tx.Init.Mode = DMA_PFCTRL;
		hdma_sdio_tx.Init.Priority = DMA_PRIORITY_LOW;
		hdma_sdio_tx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
		hdma_sdio_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
		hdma_sdio_tx.Init.MemBurst = DMA_MBURST_INC4;
		hdma_sdio_tx.Init.PeriphBurst = DMA_PBURST_INC4;
		if (HAL_DMA_Init(&hdma_sdio_tx) != HAL_OK)
			StopHier();
		else
		{
			__HAL_LINKDMA(&hsd, hdmatx, hdma_sdio_tx);
			
			/* DMA2_Stream6_IRQn interrupt configuration */
			HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 11, 0);
			HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);
			
			hsd.Instance = SDIO;
			hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
			hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
			hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
			hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
			hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
			hsd.Init.ClockDiv = 0;
			
			if (HAL_SD_Init(&hsd) != HAL_OK)
				StopHier();
			else
				HAL_SD_ConfigWideBusOperation(&hsd, SDIO_BUS_WIDE_4B);
		}
	}
}

void STM32412gDiscovery::rtcInit()
{
	__HAL_RCC_RTC_ENABLE();
	
	/**Initialize RTC Only
	 */
	hrtc.Instance = RTC;
	hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
	hrtc.Init.AsynchPrediv = 127;
	hrtc.Init.SynchPrediv = 255;
	hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
	hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	if (HAL_RTC_Init(&hrtc) != HAL_OK)
		StopHier();
}

TIM_HandleTypeDef STM32412gDiscovery::Tim14Handle;

HAL_StatusTypeDef STM32412gDiscovery::HAL_InitTick(uint32_t TickPriority)
{
	RCC_ClkInitTypeDef    clkconfig;
	uint32_t              uwTimclock = 0;
	uint32_t              uwPrescalerValue = 0;
	uint32_t              pFLatency;
	
	
	
	/* Enable TIM14 clock */
	__HAL_RCC_TIM14_CLK_ENABLE();
	
	/* Get clock configuration */
	HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);
	
	/* Compute TIM14 clock */
	uwTimclock = 2*HAL_RCC_GetPCLK1Freq();
	
	/* Compute the prescaler value to have TIM14 counter clock equal to 1MHz */
	uwPrescalerValue = (uint32_t) ((uwTimclock / 1000000) - 1);
	
	/* Initialize TIM14 */
	Tim14Handle.Instance = TIM14;
	
	/* Initialize TIMx peripheral as follow:
	+ Period = [(TIM14CLK/1000) - 1]. to have a (1/1000) s time base.
	+ Prescaler = (uwTimclock/1000000 - 1) to have a 1MHz counter clock.
	+ ClockDivision = 0
	+ Counter direction = Up
	*/
	auto &handel = Tim14Handle.Init;
	handel.Period = (1000000 / 1000) - 1;
	handel.Prescaler = uwPrescalerValue;
	handel.ClockDivision = 0;
	handel.CounterMode = TIM_COUNTERMODE_UP;

#ifndef NDEBUG
	const auto status = HAL_TIM_Base_Init(&Tim14Handle);
	assert(HAL_OK == status);
#else
	HAL_TIM_Base_Init(&Tim14Handle);
#endif
	
	/*Configure the TIM14 IRQ priority */
	HAL_NVIC_SetPriority(TIM8_TRG_COM_TIM14_IRQn, TickPriority ,4);
	/* Enable the TIM14 global Interrupt */
	HAL_NVIC_EnableIRQ(TIM8_TRG_COM_TIM14_IRQn);
	
	/* Start the TIM time Base generation in interrupt mode */
	return HAL_TIM_Base_Start_IT(&Tim14Handle);
	
}

/**
  * @brief  Suspend Tick increment.
  * @note   Disable the tick increment by disabling TIM14 update interrupt.
  * @param  None
  * @retval None
  */
void HAL_SuspendTick(void)
{
	/* Disable TIM14 update Interrupt */
	__HAL_TIM_DISABLE_IT(&STM32412gDiscovery::Tim14Handle, TIM_IT_UPDATE);
}

/**
  * @brief  Resume Tick increment.
  * @note   Enable the tick increment by Enabling TIM14 update interrupt.
  * @param  None
  * @retval None
  */
void HAL_ResumeTick(void)
{
	/* Enable TIM14 Update interrupt */
	__HAL_TIM_ENABLE_IT(&STM32412gDiscovery::Tim14Handle, TIM_IT_UPDATE);
}

#ifdef FreeRTOSGebruikt

CountingSemaphore STM32412gDiscovery::knopSems[static_cast<Teller>(KnopID::AantalKnoppen)];

void STM32412gDiscovery::isrSemafoorAfhandeling(CountingSemaphore &sem)
{
	BaseType_t nieuwePrio;
	const bool wakkerGeworden = sem.GiveFromISR(&nieuwePrio);
	if (true == wakkerGeworden)
		portYIELD_FROM_ISR(nieuwePrio);
}



#endif

DMA_HandleTypeDef STM32412gDiscovery::hdma_sdio_tx;
DMA_HandleTypeDef STM32412gDiscovery::hdma_sdio_rx;

/* C++ name mangling mag niet de interrupt handler namen veranderen, 
 * deze staan in een assembly bestand zo genoemd.
 * Derhalve hieronder met C-Linking compileren.
 */
#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************
  Cortex-M3 Processor Exceptions Handlers
******************************************************************************/

// ----------------------------------------------------------------------------
// This function handles NMI exception.
// ----------------------------------------------------------------------------
void NMI_Handler(void)
{
	while(1){;}
}

// ----------------------------------------------------------------------------
// This function handles Hard Fault exception.
// ----------------------------------------------------------------------------
void HardFault_Handler(void)
{
	// Go to infinite loop when Hard Fault exception occurs
	while(1){;}
}

// ----------------------------------------------------------------------------
// This function handles Memory Manage exception.
// ----------------------------------------------------------------------------
void MemManage_Handler(void)
{
	// Go to infinite loop when Memory Manage exception occurs
	while(1){;}
}

// ----------------------------------------------------------------------------
// This function handles Bus Fault exception.
// ----------------------------------------------------------------------------
void BusFault_Handler(void)
{
	// Go to infinite loop when Bus Fault exception occurs
	while(1){;}
}

// ----------------------------------------------------------------------------
// This function handles Usage Fault exception.
// ----------------------------------------------------------------------------
void UsageFault_Handler(void)
{
	// Go to infinite loop when Usage Fault exception occurs
	while(1){;}
}

// ----------------------------------------------------------------------------
// This function handles SVCall exception.
// ----------------------------------------------------------------------------
ZWAKGEPLAATST void SVC_Handler(void)
{
}

// ----------------------------------------------------------------------------
// This function handles Debug Monitor exception.
// ----------------------------------------------------------------------------
void DebugMon_Handler(void)
{
}

// ----------------------------------------------------------------------------
// This function handles PendSV_Handler exception.
// ----------------------------------------------------------------------------
ZWAKGEPLAATST void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
ZWAKGEPLAATST void SysTick_Handler(void)
{
	HAL_SYSTICK_IRQHandler();
}

void EXTI0_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
	
	const auto pretstaat =  BSP_JOY_GetState();

#ifndef FreeRTOSGebruikt
	if (JOY_SEL == pretstaat)
		STM32412gDiscovery::knopGedrukt(STM32412gDiscovery::KnopID::Centrum);
	else
		STM32412gDiscovery::knopGedrukt(STM32412gDiscovery::KnopID::Boven);
#else
	static const Teller knop = static_cast<Teller>((JOY_SEL == pretstaat) ? STM32412gDiscovery::KnopID::Centrum :
	                                               STM32412gDiscovery::KnopID::Boven);
	CountingSemaphore &sem = STM32412gDiscovery::knopSems[knop];
	STM32412gDiscovery::isrSemafoorAfhandeling(sem);

#endif
}

/* Knop DOWN druk interrupt */
void EXTI1_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
#ifndef FreeRTOSGebruikt
	STM32412gDiscovery::knopGedrukt(STM32412gDiscovery::KnopID::Onder);
#else
	static Teller knop = static_cast<Teller>(STM32412gDiscovery::KnopID::Onder);
	CountingSemaphore &sem = STM32412gDiscovery::knopSems[knop];
	STM32412gDiscovery::isrSemafoorAfhandeling(sem);

#endif

}

/* links of rechts ingedrukt */
void EXTI15_10_IRQHandler(void)
{

#ifndef FreeRTOSGebruikt
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_14) != RESET)
	{
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
		STM32412gDiscovery::knopGedrukt(STM32412gDiscovery::KnopID::Rechts);
	}

	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_15) != RESET)
	{
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
		STM32412gDiscovery::knopGedrukt(STM32412gDiscovery::KnopID::Links);
	}
#else
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_14) != RESET)
	{
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
		static constexpr auto knop = static_cast<Teller>(STM32412gDiscovery::KnopID::Rechts);
		static constexpr auto &sem = STM32412gDiscovery::knopSems[knop];
		STM32412gDiscovery::isrSemafoorAfhandeling(sem);
	}
	
	if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_15) != RESET)
	{
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
		static constexpr auto knop = static_cast<Teller>(STM32412gDiscovery::KnopID::Links);
		static constexpr auto &sem = STM32412gDiscovery::knopSems[knop];
		STM32412gDiscovery::isrSemafoorAfhandeling(sem);
	}

#endif
}


void TIM8_BRK_TIM12_IRQHandler(void)
{
	/* wacht timer */
	HAL_TIM_IRQHandler(&(STM32412gDiscovery::Tim12Handle));
	
	STM32412gDiscovery::timer2IsAfgelopen();
}

void TIM8_UP_TIM13_IRQHandler(void)
{
	/* blauwe led knipper */
	HAL_TIM_IRQHandler(&(STM32412gDiscovery::Tim13Handle));
	BSP_LED_Toggle(LED4);
}



void TIM8_TRG_COM_TIM14_IRQHandler(void)
{
	/**
     * @brief This function handles TIM8 trigger and commutation interrupts and TIM14 global interrupt.
     * Deze funktie wordt gebruikt voor de HAL Tick funktie.
     */
	
	/* USER CODE BEGIN TIM8_TRG_COM_TIM14_IRQn 0 */
	
	/* USER CODE END TIM8_TRG_COM_TIM14_IRQn 0 */
	HAL_TIM_IRQHandler(&(STM32412gDiscovery::Tim14Handle));
	HAL_IncTick();
	/* USER CODE BEGIN TIM8_TRG_COM_TIM14_IRQn 1 */
	
	/* USER CODE END TIM8_TRG_COM_TIM14_IRQn 1 */
}



/**
* @brief This function handles DMA2 stream3 global interrupt.
*/
void DMA2_Stream3_IRQHandler(void)
{
	/* USER CODE BEGIN DMA2_Stream3_IRQn 0 */
	
	/* USER CODE END DMA2_Stream3_IRQn 0 */
	HAL_DMA_IRQHandler(&STM32412gDiscovery::hdma_sdio_rx);
	/* USER CODE BEGIN DMA2_Stream3_IRQn 1 */
	
	/* USER CODE END DMA2_Stream3_IRQn 1 */
}

/**
* @brief This function handles DMA2 stream6 global interrupt.
*/
void DMA2_Stream6_IRQHandler(void)
{
	/* USER CODE BEGIN DMA2_Stream6_IRQn 0 */
	
	/* USER CODE END DMA2_Stream6_IRQn 0 */
	HAL_DMA_IRQHandler(&STM32412gDiscovery::hdma_sdio_tx);
	/* USER CODE BEGIN DMA2_Stream6_IRQn 1 */
	
	/* USER CODE END DMA2_Stream6_IRQn 1 */
}


#ifdef __cplusplus
}
#endif

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	   ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	
	StopHier();
}
#endif



/* Disk IO funkties van FatFS koppelen aan QSPI geheugen op board. */
int assign_drives (int, int)
{
	return(0);
}

DSTATUS QSPI_disk_initialize ()
{
	/* gebeurt als in de STM32412gDiscovery klasse */
	return(0);
}

DSTATUS QSPI_disk_status ()
{
	return(0);
}

static const auto BLOCK_SIZE = 512;

DRESULT QSPI_disk_read (BYTE *buff,		/* Data buffer to store read data */
                        DWORD sector,	    /* Sector address (LBA) */
                        UINT count)		/* Number of sectors to read (1..128) */
{
	if ((DWORD)buff & 3)
	{
		DRESULT res = RES_ERROR;
		DWORD scratch[BLOCK_SIZE / 4];
		
		while (count--)
		{
			/* recursie !!! */
			res = QSPI_disk_read(reinterpret_cast<BYTE *>(scratch),
			                     sector++,
			                     1);
			
			if (res != RES_OK) {
				break;
			}
			
			memcpy(buff, scratch, BLOCK_SIZE);
			
			buff += BLOCK_SIZE;
		}
		
		return res;
	}
	
	const auto aantal = BLOCK_SIZE*count;
	const auto adres = (sector<<9);
	const auto ret = BSP_QSPI_Read(buff,adres,aantal);
	//SD_Error Status = SD_OSReadMultiBlocks(buff, sector << 9, BLOCK_SIZE, count);
	return((QSPI_OK == ret) ? RES_OK : RES_ERROR);
}

#if	_READONLY == 0
DRESULT QSPI_disk_write (const BYTE *buff,	/* Data to be written */
                         DWORD sector,		/* Sector address (LBA) */
                         UINT count			/* Number of sectors to write (1..128) */
)
{
	if ((DWORD)buff & 3) {
		DRESULT res=RES_OK;
		DWORD scratch[BLOCK_SIZE / 4];
		
		while (count--) {
			memcpy(scratch, buff, BLOCK_SIZE);
			
			/* recursief !!! */
			res = QSPI_disk_write(reinterpret_cast<BYTE *>(scratch),
			                      sector++, 1);
			
			if (res != RES_OK)
			{
				break;
			}
			
			buff += BLOCK_SIZE;
		}
		
		return(res);
	}
	const auto aantal = BLOCK_SIZE*count;
	const auto adres = (sector<<9);
	const auto ret = BSP_QSPI_Write(const_cast<UInt8 *>(buff),
	                                adres,aantal);
	
	return((QSPI_OK == ret) ? RES_OK : RES_ERROR);
}

#endif
DRESULT QSPI_disk_ioctl (BYTE, BYTE, void*)
{
	return(RES_OK);
}



/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat=RES_NOTRDY;
	
	switch (pdrv) {
		case DEV_RAM :
			//result = RAM_disk_status();
			
			break;
		
		case DEV_QSPI :
			stat = QSPI_disk_status();
			
			break;
		
		case DEV_MMC :
			//result = MMC_disk_status();
			
			break;
		
		case DEV_USB :
			//result = USB_disk_status();
			
			break;
		
		default:
			StopHier();
			break;
	}
	return (stat);
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat=STA_NOINIT;
	
	switch (pdrv) {
		case DEV_RAM :
			//result = RAM_disk_initialize();
			
			break;
		
		case DEV_QSPI :
			stat = QSPI_disk_initialize();
			
			break;
		
		case DEV_MMC :
			//result = MMC_disk_initialize();
			
			break;
		
		case DEV_USB :
			//result = USB_disk_initialize();
			
			break;
		
		default:
			StopHier();
			break;
	}
	return(stat);
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT result=RES_NOTRDY;
	
	switch (pdrv) {
		case DEV_RAM :
			// translate the arguments here
			
			//result = RAM_disk_read(buff, sector, count);
			
			break;
		
		case DEV_QSPI :
			// translate the arguments here
			
			result = QSPI_disk_read(buff, sector, count);
			
			break;
		
		case DEV_MMC :
			// translate the arguments here
			
			//result = MMC_disk_read(buff, sector, count);
			
			break;
		
		case DEV_USB :
			// translate the arguments here
			
			//result = USB_disk_read(buff, sector, count);
			
			break;
		
		default:
			StopHier();
			break;
	}
	
	return(result);
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT result = RES_NOTRDY;
	
	switch (pdrv) {
		case DEV_RAM :
			// translate the arguments here
			
			//result = RAM_disk_write(buff, sector, count);
			
			break;
		
		case DEV_QSPI:
			// translate the arguments here
			
			result = QSPI_disk_write(buff, sector, count);
			
			break;
		
		case DEV_MMC :
			// translate the arguments here
			
			//result = MMC_disk_write(buff, sector, count);
			
			// translate the reslut code here
			
			break;
		
		case DEV_USB :
			// translate the arguments here
			
			//result = USB_disk_write(buff, sector, count);
			
			break;
		
		default:
			StopHier();
			break;
	}
	
	return(result);
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res=RES_OK;
	
	switch (pdrv)
	{
		case DEV_RAM :
			
			// Process of the command for the RAM drive
			
			return res;
		
		case DEV_MMC :
			
			// Process of the command for the MMC/SD card
			
			return res;
		
		case DEV_USB :
			
			// Process of the command the USB drive
			
			return res;
		
		default:
			StopHier();
			break;
	}
	
	return RES_PARERR;
}
