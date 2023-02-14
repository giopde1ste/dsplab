/******************************************************************************
 * Project        : HAN ESE Project 3 - Regeltechniek
 * File           : Basisklasse en startup routines van de STM32

 * Copyright      : 2010-2013 HAN Embedded Systems Engineering Ewout Boks en Hugo Arends
 ******************************************************************************
  Change History:

    Version 2.0 - September 2011
    > begin .
 $Id: ESEDigProcesController.cpp 3452 2019-03-20 16:34:42Z ewout $     
******************************************************************************/

#include <ESEDigProcesController.h>
#include <algdef.h>

#include <assert.h>
#include <stm32f3xx_hal_tim.h>

STM32DigProcesController::STM32DigProcesController(const UInt32 cw) : crystalWaarde(cw)
{
	assert((cw==8000000) || (cw==16000000));

	SYSCLKConfig_STOP();

	/* Use systick as time base source and configure 1ms tick (default clock after Reset is HSI) */
	HAL_InitTick(TICK_INT_PRIORITY);

	gpioInit();
	i2cInit();
	tim1Init();
	tim2Init();
	adcInit();
	dacInit();
}


void STM32DigProcesController::wachtFunktie(const bool stopMode)
{
	if (true == stopMode)
	{
		HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON,PWR_STOPENTRY_WFI);

		/* STM32 gaat verder na knop interrupt */
		SYSCLKConfig_STOP();  /* breng PLL en HSE weer tot leven */
	}
	else  /* wacht op interrupt */
	{
		slaapMode();
	}
}


void STM32DigProcesController::slaapMode() const
{
	/* This option is used to ensure that store operations are completed */
#if defined ( __CC_ARM   )
	__force_stores();
#endif
	/* Request Wait For Interrupt */
	__WFI();
}


/**
 * @brief  Configures system clock after wake-up from STOP: enable HSE, PLL
 *         and select PLL as system clock source.
 * @param  None
 * @retval None
 */
void STM32DigProcesController::SYSCLKConfig_STOP()
{
/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = crystalwaarde
  *            HSE PREDIV                     = 1
  *            PLLMUL                         = 9
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;


#ifdef USE_FULL_ASSERT
	uint32_t ret = HAL_OK;
#endif /* USE_FULL_ASSERT */

	/* Enable HSE Oscillator and activate PLL with HSE as source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = ((crystalWaarde == 8000000) ? RCC_HSE_PREDIV_DIV1 : RCC_HSE_PREDIV_DIV2);
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;

#ifdef USE_FULL_ASSERT
	ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
	if (ret != HAL_OK)
	{
		assert_failed((uint8_t *) __FILE__, __LINE__);
	}
#else
	HAL_RCC_OscConfig(&RCC_OscInitStruct);
#endif /* USE_FULL_ASSERT */

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
	   clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 |
								   RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

#ifdef USE_FULL_ASSERT
	ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
	if (ret != HAL_OK)
	{
		assert_failed((uint8_t *) __FILE__, __LINE__);
	}
#else
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
#endif /* USE_FULL_ASSERT */

	RCC_PeriphCLKInitTypeDef PeriphClkInit;

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
	PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
		StopHier();
	else
	{
		/**Enables the Clock Security System  */
		HAL_RCC_EnableCSS();

		/**Configure the Systick interrupt time
		*/
		HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

		/**Configure the Systick
		*/
		HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

		/* SysTick_IRQn interrupt configuration */
		HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
	}
}


void STM32DigProcesController::initWaakhond(const UInt16 prescaler,
											const UInt16 venster)
{
	waakhond.Init.Prescaler = prescaler;
	waakhond.Init.Reload = venster;

	if (HAL_OK != HAL_IWDG_Init(&waakhond))
		StopHier();

}

void STM32DigProcesController::aaiWaakhond()
{
	HAL_IWDG_Refresh(&waakhond);
}


const UInt16 STM32DigProcesController::kleurKanaal[STM32DigProcesController::LaatsteKleur] = { TIM_CHANNEL_1,
																							   TIM_CHANNEL_2 };


void STM32DigProcesController::zetLED(const LEDKleur kleur,
									  const Schakelaar stand,
									  const KnipperLEDStand knipper)
{
	assert(kleur < LaatsteKleur);

	const UInt16 kanaal = kleurKanaal[kleur];
	TIM_OC_InitTypeDef &ocConfig = ledOC[kleur];

	switch(stand)
	{
		case Schakelaar::Aan:
		{
			zettim1Frequentie(knipper);
			if (KnipperLED_GeenKnipper == knipper)
			{
				ocConfig.OCMode = TIM_OCMODE_ACTIVE;
				if (HAL_TIM_OC_ConfigChannel(&htim1, &ocConfig, kanaal) != HAL_OK)
					StopHier();
				else
				{
					HAL_TIM_OC_Start(&htim1, kanaal);
				}
			}
			else
			{
				ocConfig.OCMode = TIM_OCMODE_PWM1;
				ocConfig.Pulse = htim1.Init.Period/2;
				if (HAL_TIM_PWM_ConfigChannel(&htim1, &ocConfig, kanaal) != HAL_OK)
					StopHier();
				else
				{
					HAL_TIM_PWM_Start(&htim1, kanaal);
				}
			}
		}
			break;

		case Schakelaar::Uit:
		{
			ocConfig.OCMode = TIM_OCMODE_INACTIVE;
			if (HAL_TIM_OC_ConfigChannel(&htim1, &ocConfig, kanaal) != HAL_OK)
				StopHier();
			else
			{
				HAL_TIM_OC_Start(&htim1, kanaal);
			}
		}
			break;

		default:
			StopHier();
			break;
	}
}

void STM32DigProcesController::ontvang(const DataPakket<UInt8> &ontvangPakket)
{
	I2C_HandleTypeDef * const i2c =  &hi2c1 ;

	UInt8 * const inPtr = &(ontvangPakket[0]);

	i2c1RxSem=false;

	HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
	HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);

	HAL_I2C_Slave_Receive_DMA(i2c,
							  inPtr,
							  ontvangPakket.geefGrootte());

	while (false == i2c1RxSem);

	HAL_NVIC_DisableIRQ(DMA1_Channel6_IRQn);
	HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);
}

void STM32DigProcesController::verstuur(const DataPakket<UInt8> &verzendPakket)
{
	I2C_HandleTypeDef * const i2c =  &hi2c1 ;

	UInt8 * const uitPtr = &(verzendPakket[0]);

	i2c1RxSem=false;

	HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
	HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

	HAL_I2C_Slave_Transmit_DMA(i2c,
			//				(0xdc>>1),
							   uitPtr,
							   verzendPakket.geefGrootte());

	while (false == i2c1TxSem);

	HAL_NVIC_DisableIRQ(DMA1_Channel7_IRQn);
	HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);
}

void STM32DigProcesController::startADC(const UInt32 sampTijd)
{
	adcSem=false;

	HAL_ADC_Start_IT(&hadc1);
	NVIC_EnableIRQ(ADC1_IRQn);

#ifdef BoardVersie_1_0
	HAL_TIM_Base_Start(&htim3);
#else
	HAL_TIM_Base_Start(&htim2);
#endif
}

void STM32DigProcesController::wachtOpConversie()
{
	//while (false == adcSem);

	while((HAL_IS_BIT_CLR(HAL_ADC_GetState(&hadc1),
						  HAL_ADC_STATE_REG_EOC)));

	const UInt16 convWaarde = HAL_ADC_GetValue(&hadc1);
	adcSem=false;
	adcWaarde = convWaarde;
}

void STM32DigProcesController::stopADC()
{
#ifdef BoardVersie_1_0
	HAL_TIM_Base_Stop(&htim3);
#else
	HAL_TIM_Base_Stop(&htim2);
#endif
	HAL_ADC_Stop_IT(&hadc1);
	NVIC_DisableIRQ(ADC1_IRQn);
}


void STM32DigProcesController::startDAC()
{
	HAL_DAC_Start(&hdac1,DAC_CHANNEL_1);
}

void STM32DigProcesController::zetDACwaarde(const UInt16 waarde)
{
	HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_1,DAC_ALIGN_12B_R,waarde);
}

void STM32DigProcesController::stopDAC()
{
	HAL_DAC_Stop(&hdac1,DAC_CHANNEL_1);
}


UInt32 STM32DigProcesController::revEndianess(const UInt32 value) const
{
	UInt32 result;
	__ASM volatile ("rev %0, %1" : "=r" (result) : "r" (value) );
	return(result);
}

void STM32DigProcesController::gpioInit()
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	/*Configure LED pins : PA0 PA1 */
	GPIO_InitStruct.Pin = GPIO_PIN_0+GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : PB0 PB1 */
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pins : PB2 PB3 */
	GPIO_InitStruct.Pin = GPIO_PIN_2+GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}


void STM32DigProcesController::i2cInit()
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* Peripheral clock enable */
	__HAL_RCC_I2C1_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/**I2C1 GPIO Configuration
   PB6     ------> I2C1_SCL
   PB7     ------> I2C1_SDA
   */
	GPIO_InitStruct.Pin = GPIO_PIN_6+GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


	hi2c1.Instance = I2C1;
	hi2c1.Init.Timing = 0x0000020B;
	hi2c1.Init.OwnAddress1 = (EigenI2CAdres<<1);
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK)
		StopHier();
	else
	{
		/* Filtering op de lijn */
		HAL_I2CEx_ConfigAnalogFilter(&hi2c1,I2C_ANALOGFILTER_ENABLE);
		//	HAL_I2CEx_ConfigDigitalFilter(&hi2c1,0x04);

		__HAL_RCC_DMA1_CLK_ENABLE();
		/* Peripheral DMA init*/

		hdma_i2c1_rx.Instance = DMA1_Channel7;
		hdma_i2c1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_i2c1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_i2c1_rx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_i2c1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_i2c1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_i2c1_rx.Init.Mode = DMA_NORMAL;
		hdma_i2c1_rx.Init.Priority = DMA_PRIORITY_HIGH;
		if (HAL_DMA_Init(&hdma_i2c1_rx) != HAL_OK)
			StopHier();
		else
		{
			__HAL_LINKDMA(&hi2c1, hdmarx, hdma_i2c1_rx);
		}

		hdma_i2c1_tx.Instance = DMA1_Channel6;
		hdma_i2c1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma_i2c1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_i2c1_tx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_i2c1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_i2c1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_i2c1_tx.Init.Mode = DMA_NORMAL;
		hdma_i2c1_tx.Init.Priority = DMA_PRIORITY_MEDIUM;
		if (HAL_DMA_Init(&hdma_i2c1_tx) != HAL_OK)
			StopHier();
		else
		{
			__HAL_LINKDMA(&hi2c1, hdmatx, hdma_i2c1_tx);
			/* Peripheral interrupt init */
			HAL_NVIC_SetPriority(I2C1_EV_IRQn, 5, 0);
			HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
			HAL_NVIC_SetPriority(I2C1_ER_IRQn, 5, 0);
			HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
			HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 3, 0);
			HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 3, 0);
		}
	}
}

void STM32DigProcesController::adcInit()
{
	/* Peripheral clock enable */
	__HAL_RCC_ADC1_CLK_ENABLE();
	
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
	hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T2_TRGO;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 1;
	hadc1.Init.DMAContinuousRequests = ENABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc1.Init.LowPowerAutoWait = DISABLE;
	hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
	if (HAL_ADC_Init(&hadc1) != HAL_OK)
	{
		StopHier();
	}
	else
	{
		ADC_ChannelConfTypeDef sConfig;
		/**Configure Regular Channel
		*/
		sConfig.Channel = ADC_CHANNEL_10;
		sConfig.Rank = ADC_REGULAR_RANK_1;
		sConfig.SingleDiff = ADC_SINGLE_ENDED;
		sConfig.SamplingTime = ADC_SAMPLETIME_19CYCLES_5;
		sConfig.OffsetNumber = ADC_OFFSET_NONE;
		sConfig.Offset = 0;
		if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
		{
			StopHier();
		}
		else
		{
			GPIO_InitTypeDef GPIO_InitStruct;

			/**ADC1 GPIO Configuration
			PA6     ------> ADC1_IN10
			*/
			__HAL_RCC_GPIOA_CLK_ENABLE();

			GPIO_InitStruct.Pin = GPIO_PIN_6;
			GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

			/* Peripheral DMA init*/

			hdma_adc1.Instance = DMA1_Channel1;
			hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
			hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
			hdma_adc1.Init.MemInc = DMA_MINC_DISABLE;
			hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
			hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
			hdma_adc1.Init.Mode = DMA_CIRCULAR;
			hdma_adc1.Init.Priority = DMA_PRIORITY_VERY_HIGH;
			if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
				StopHier();

			__HAL_LINKDMA(&hadc1, DMA_Handle, hdma_adc1);
			/* DMA1_Channel1_IRQn interrupt configuration */
			HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 5, 0);
			HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
		}
	}
}


void STM32DigProcesController::dacInit()
{
	/* Peripheral clock enable */
	__HAL_RCC_DAC1_CLK_ENABLE();
	/**DAC Initialization
	*/
	hdac1.Instance = DAC;
	if (HAL_DAC_Init(&hdac1) != HAL_OK)
		StopHier();
	else
	{
		DAC_ChannelConfTypeDef sConfig;
		/**DAC channel OUT1 config
		*/
		sConfig.DAC_Trigger = DAC_TRIGGER_SOFTWARE;
		sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
		if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK)
			StopHier();
		else
		{
			/* Peripheral DMA init*/

			hdma_dac_ch1.Instance = DMA1_Channel3;
			hdma_dac_ch1.Init.Direction = DMA_MEMORY_TO_PERIPH;
			hdma_dac_ch1.Init.PeriphInc = DMA_PINC_DISABLE;
			hdma_dac_ch1.Init.MemInc = DMA_MINC_ENABLE;
			hdma_dac_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
			hdma_dac_ch1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
			hdma_dac_ch1.Init.Mode = DMA_NORMAL;
			hdma_dac_ch1.Init.Priority = DMA_PRIORITY_HIGH;
			if (HAL_DMA_Init(&hdma_dac_ch1) != HAL_OK)
			{
				StopHier();
			}
			else
			{
				__HAL_DMA_REMAP_CHANNEL_ENABLE(HAL_REMAPDMA_TIM6_DAC1_CH1_DMA1_CH3);

				__HAL_LINKDMA(&hdac1, DMA_Handle1, hdma_dac_ch1);
				/* DMA1_Channel3_IRQn interrupt configuration */
				HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 5, 0);
				HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

				HAL_NVIC_SetPriority(ADC1_IRQn, 5, 0);

				__HAL_RCC_GPIOA_CLK_ENABLE();
				GPIO_InitTypeDef GPIO_InitStruct;
				/**DAC1 GPIO Configuration
				PA4     ------> DAC1_OUT1
				*/
				GPIO_InitStruct.Pin = GPIO_PIN_4;
				GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
				GPIO_InitStruct.Pull = GPIO_NOPULL;
				HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
			}
		}
	}
}

void STM32DigProcesController::zettim1Frequentie(const KnipperLEDStand stand)
{
	assert(stand < KnipperLED_Laatste);

	static const UInt16 standen[] = { 45000,45000,22500,11250,4500 };

	htim1.Init.Period = standen[stand];

	if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
		StopHier();
}


void STM32DigProcesController::tim1Init()
{
	__HAL_RCC_TIM1_CLK_ENABLE();


	htim1.Instance = TIM1;
	htim1.Init.Prescaler =1800;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 45000;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
		StopHier();
	else
	{
		TIM_ClockConfigTypeDef sClockSourceConfig;
		TIM_MasterConfigTypeDef sMasterConfig;

		sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
		if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
			StopHier();

		if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
			StopHier();

		sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
		sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
		sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
		if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
			StopHier();

		ledOC[0].OCMode = TIM_OCMODE_PWM1;
		ledOC[0].Pulse = 500;
		ledOC[0].OCPolarity = TIM_OCPOLARITY_HIGH;
		ledOC[0].OCFastMode = TIM_OCFAST_DISABLE;

		ledOC[1] = ledOC[0];

		if (HAL_TIM_PWM_ConfigChannel(&htim1, &ledOC[0], TIM_CHANNEL_1) != HAL_OK)
			StopHier();

		if (HAL_TIM_PWM_ConfigChannel(&htim1, &ledOC[1], TIM_CHANNEL_2) != HAL_OK)
			StopHier();
		else
		{
			TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;
			sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
			sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
			sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
			sBreakDeadTimeConfig.DeadTime = 0;
			sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
			sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
			sBreakDeadTimeConfig.BreakFilter = 0;
			sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
			sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
			sBreakDeadTimeConfig.Break2Filter = 0;
			sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
			if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
			{
				StopHier();
			}
			else
			{
				__HAL_RCC_GPIOA_CLK_ENABLE();
				GPIO_InitTypeDef GPIO_InitStruct;
				/**TIM1 GPIO Configuration
					 PA8+PA9     ------> TIM1_CH1+TIM1_CH2
				   */
				GPIO_InitStruct.Pin = GPIO_PIN_8 + GPIO_PIN_9;
				GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
				GPIO_InitStruct.Pull = GPIO_NOPULL;
				GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
				GPIO_InitStruct.Alternate = GPIO_AF6_TIM1;
				HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
			}
		}
	}

}

void STM32DigProcesController::tim2Init()
{
	__HAL_RCC_TIM2_CLK_ENABLE();

#ifdef BoardVersie_1_0
	htim2.Instance = TIM2;
	htim2.Init.Prescaler =100;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 0xffff;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
		StopHier();
	else
	{
		TIM_ClockConfigTypeDef sClockSourceConfig;
		TIM_MasterConfigTypeDef sMasterConfig;

		sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
		if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
			StopHier();

		if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
			StopHier();

		sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
		sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
		if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
			StopHier();

		ledOC[0].OCMode = TIM_OCMODE_PWM1;
		ledOC[0].Pulse = 0xfffe;
		ledOC[0].OCPolarity = TIM_OCPOLARITY_HIGH;
		ledOC[0].OCFastMode = TIM_OCFAST_DISABLE;

		ledOC[1] = ledOC[0];

		if (HAL_TIM_PWM_ConfigChannel(&htim2, &ledOC[0], TIM_CHANNEL_1) != HAL_OK)
			StopHier();

		if (HAL_TIM_PWM_ConfigChannel(&htim2, &ledOC[1], TIM_CHANNEL_2) != HAL_OK)
			StopHier();
		else
		{
			GPIO_InitTypeDef GPIO_InitStruct;
			/**TIM2 GPIO Configuration
	  		   PA0+PA1     ------> TIM2_CH1+TIM2_CH2
	  		 */
			GPIO_InitStruct.Pin = GPIO_PIN_0+GPIO_PIN_0;
			GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
			GPIO_InitStruct.Alternate = GPIO_AF2_TIM2;
			HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		}
	}
#elif defined(BoardVersie_1_1)

	TIM_ClockConfigTypeDef sClockSourceConfig;

	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 72;  /* 72 Mhz --> 1 Mhz */
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 1000000;  /* 1 seconde */
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
		StopHier();
	else
	{
		sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
		if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
			StopHier();
		else
		{
			TIM_MasterConfigTypeDef sMasterConfig;

			sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE; //TIM_TRGO_OC1REF;
			sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
			sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
			if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
				StopHier();
			else
			{
				/* Peripheral interrupt init */
				HAL_NVIC_SetPriority(TIM2_IRQn, 5, 0);
				HAL_NVIC_EnableIRQ(TIM2_IRQn);
			}
		}
	}
#endif
}

/**
  * @brief  This function configures the TIM17 as a time base source.
  *         The time source is configured  to have 1ms time base with a dedicated
  *         Tick interrupt priority.
  * @note   This function is called  automatically at the beginning of program after
  *         reset by HAL_Init() or at any time when clock is configured, by HAL_RCC_ClockConfig().
  * @param  TickPriority: Tick interrupt priorty.
  * @retval HAL status
  */
HAL_StatusTypeDef STM32DigProcesController::HAL_InitTick(uint32_t TickPriority)
{
	RCC_ClkInitTypeDef    clkconfig;
	uint32_t              uwTimclock = 0;
	uint32_t              uwPrescalerValue = 0;
	uint32_t              pFLatency;

	/*Configure the TIM17 IRQ priority */
	HAL_NVIC_SetPriority(TIM1_TRG_COM_TIM17_IRQn, TickPriority ,0);

	/* Enable the TIM17 global Interrupt */
	HAL_NVIC_EnableIRQ(TIM1_TRG_COM_TIM17_IRQn);

	/* Enable TIM17 clock */
	__HAL_RCC_TIM17_CLK_ENABLE();

	/* Get clock configuration */
	HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

	/* Compute TIM17 clock */
	uwTimclock = HAL_RCC_GetPCLK2Freq();

	/* Compute the prescaler value to have TIM17 counter clock equal to 1MHz */
	uwPrescalerValue = (uint32_t) ((uwTimclock / 1000000) - 1);

	/* Initialize TIM17 */
	htim17.Instance = TIM17;

	/* Initialize TIMx peripheral as follow:
	+ Period = [(TIM17CLK/1000) - 1]. to have a (1/1000) s time base.
	+ Prescaler = (uwTimclock/1000000 - 1) to have a 1MHz counter clock.
	+ ClockDivision = 0
	+ Counter direction = Up
	*/
	htim17.Init.Period = (1000000 / 1000) - 1;
	htim17.Init.Prescaler = uwPrescalerValue;
	htim17.Init.ClockDivision = 0;
	htim17.Init.CounterMode = TIM_COUNTERMODE_UP;
	if(HAL_TIM_Base_Init(&htim17) == HAL_OK)
	{
		/* Start the TIM time Base generation in interrupt mode */
		return HAL_TIM_Base_Start_IT(&htim17);
	}

	/* Return function status */
	return HAL_ERROR;
}

/**
  * @brief  Suspend Tick increment.
  * @note   Disable the tick increment by disabling TIM17 update interrupt.
  * @param  None
  * @retval None
  */
void STM32DigProcesController::HAL_SuspendTick(void)
{
	/* Disable TIM17 update Interrupt */
	__HAL_TIM_DISABLE_IT(&htim17, TIM_IT_UPDATE);
}

/**
  * @brief  Resume Tick increment.
  * @note   Enable the tick increment by Enabling TIM17 update interrupt.
  * @param  None
  * @retval None
  */
void STM32DigProcesController::HAL_ResumeTick(void)
{
	/* Enable TIM17 Update interrupt */
	__HAL_TIM_ENABLE_IT(&htim17, TIM_IT_UPDATE);
}

DMA_HandleTypeDef STM32DigProcesController::hdma_i2c1_tx;
DMA_HandleTypeDef STM32DigProcesController::hdma_i2c1_rx;

TIM_HandleTypeDef STM32DigProcesController::htim17;


I2C_HandleTypeDef STM32DigProcesController::hi2c1;

bool STM32DigProcesController::i2c1RxSem;
bool STM32DigProcesController::i2c1TxSem;


ADC_HandleTypeDef STM32DigProcesController::hadc1;
bool STM32DigProcesController::adcSem;

DigProcesController::DigProcesController(const UInt32 cw) : STM32DigProcesController(cw)
{

}

void DigProcesController::i2cTest()
{
	/**I2C1 GPIO Configuration
	PB6     ------> I2C1_SCL --> Bruin
	PB7     ------> I2C1_SDA --> Zwart */

	const DataPakket<UInt8> testPakket(i2cOntvangstBuffer,10);
	ontvang(testPakket);

	const DataPakket<UInt8> test2Pakket(i2cOntvangstBuffer+10,10);
	ontvang(test2Pakket);
}

void DigProcesController::adcTest()
{
	startADC(10000);

	for (Teller i=0;i<10;i++)
	{
		wachtOpConversie();

	}

	stopADC();

}


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
void SVC_Handler(void)
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
void PendSV_Handler(void)
{
}

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
	/* USER CODE BEGIN SysTick_IRQn 0 */

	/* USER CODE END SysTick_IRQn 0 */

	/* USER CODE BEGIN SysTick_IRQn 1 */

	/* USER CODE END SysTick_IRQn 1 */
}

/**
* @brief This function handles I2C1 event interrupt.
*/
void I2C1_EV_IRQHandler(void)
{
	HAL_I2C_EV_IRQHandler(&STM32DigProcesController::hi2c1);
}


void I2C1_ER_IRQHandler(void)
{
	HAL_I2C_ER_IRQHandler(&STM32DigProcesController::hi2c1);
}

void DMA1_Channel6_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&(STM32DigProcesController::hdma_i2c1_rx));

	if (HAL_DMA_STATE_READY == STM32DigProcesController::hdma_i2c1_rx.State)
	{
		STM32DigProcesController::i2c1RxSem = true;
	}
}

void DMA1_Channel7_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&(STM32DigProcesController::hdma_i2c1_tx));
	if(HAL_DMA_STATE_READY == STM32DigProcesController::hdma_i2c1_tx.State)
	{
		STM32DigProcesController::i2c1TxSem = true;
	}
}

/**
* @brief This function handles ADC and COMP interrupts (COMP interrupts through EXTI lines 21 and 22).
*/
void ADC1_IRQHandler(void)
{
	HAL_ADC_IRQHandler(&(STM32DigProcesController::hadc1));
}

/**
* @brief This function handles TIM1 trigger, commutation and TIM17 interrupts.
*/
void TIM1_TRG_COM_TIM17_IRQHandler(void)
{
	/* USER CODE BEGIN TIM1_TRG_COM_TIM17_IRQn 0 */

	/* USER CODE END TIM1_TRG_COM_TIM17_IRQn 0 */
	//HAL_TIM_IRQHandler(&htim1);
	HAL_TIM_IRQHandler(&(STM32DigProcesController::htim17));
	HAL_IncTick();
	/* USER CODE BEGIN TIM1_TRG_COM_TIM17_IRQn 1 */

	/* USER CODE END TIM1_TRG_COM_TIM17_IRQn 1 */
}

/**
* @brief This function handles TIM2 global interrupt.
*/
void TIM2_IRQHandler(void)
{
	/* USER CODE BEGIN TIM2_IRQn 0 */

	/* USER CODE END TIM2_IRQn 0 */
	//HAL_TIM_IRQHandler(&htim2);
	/* USER CODE BEGIN TIM2_IRQn 1 */

	/* USER CODE END TIM2_IRQn 1 */
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
