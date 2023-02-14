/*!
@file
Implementatie van de c++ schil voor gebruik van de ESE Board kit.
@version $Rev: 3649 $
@author $Author: ewout $

@copyright Copyright 2017-2018 Hogeschool van Arnhem en Nijmegen, ir drs E.J Boks

$URL: http://svn.hanese.nl/docenten/svn/ewout/vakken/algemeen/software/STM32/STM32Basis/HAL/gemeenschappelijk/Third_Party/ESE_BSP/ESEPracticumboard_RGT+DSB/ESEBoardController.cpp $
$Id: ESEBoardController.cpp 3649 2019-10-03 16:34:04Z ewout $
************************************************************************/

#include <ESEBoardController.h>
#include <algdef.h>

#include <assert.h>
#include <stm32f4xx_hal_uart.h>
#include <stm32f4xx_ll_spi.h>

#ifndef BoardVersie
#error "Definieer op command line (CMakeLists.txt) voor welke boardversie wordt gebouwd."
#else

#ifndef HAL_RTC_MODULE_ENABLED
#error "HAL_RTC_MODULE_ENABLED moet aan staan voor dit BSP."
#endif

STM32BoardController::STM32BoardController(const UInt32 freq) : BoardControllerBasis(freq)
{
#ifdef SeggerRTTAanwezig
	/* configureer de Segger RTT driver */
	SEGGER_RTT_Init();
	
	auto gelukt = SEGGER_RTT_ConfigUpBuffer(TerminalBufferIndex,
	                                        "Terminal buffer", RTTtekstBuffer,
	                                        RTTBufferGrootte,
		//SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
		                                    SEGGER_RTT_MODE_NO_BLOCK_TRIM);
	if (false == (gelukt <0))
	{
		gelukt = SEGGER_RTT_ConfigUpBuffer(LoggingBufferIndex,
		                                   "Logging Buffer",
		                                   RTTloggingBuffer,
		                                   RTTBufferGrootte,
			//SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
			                               SEGGER_RTT_MODE_NO_BLOCK_TRIM);
		
		if (false == (gelukt <0))
		{
			SEGGER_RTT_WriteString(TerminalBufferIndex,
			                       "(STM32BoardController::STM32BoardController) Segger RTT operationeel.\n");
		}
	}
#endif

	/* initialiseer de peripheral drivers */
	gpioInit();

	spiInit();
	i2cInit();
	uart2Init();
	usart6Init();
}

SPI_HandleTypeDef STM32BoardController::hspi1;
SPI_HandleTypeDef STM32BoardController::hspi2;

#ifndef FreeRTOSGebruikt
volatile bool STM32BoardController::spi1Sem;
volatile bool STM32BoardController::spi2Sem;
volatile bool STM32BoardController::i2c1Sem[AantalSems];
volatile bool STM32BoardController::i2c2Sem[AantalSems];
volatile bool STM32BoardController::usart2Sem[AantalSems];
volatile bool STM32BoardController::usart6Sem[AantalSems];
#else
CountingSemaphore STM32BoardController::spi1Sem,STM32BoardController::spi2Sem;
CountingSemaphore STM32BoardController::usart2Sem[AantalSems];
CountingSemaphore STM32BoardController::usart6Sem[AantalSems];
CountingSemaphore STM32BoardController::i2c1Sem[STM32BoardController::AantalSems];
CountingSemaphore STM32BoardController::i2c2Sem[STM32BoardController::AantalSems];
CountingSemaphore STM32BoardController::knopSems[static_cast<Teller>(KnopID::AantalKnoppen)];
#endif

UInt32 STM32BoardController::verstuur(const STM32BoardController::BoardSPI eh, const UInt32 waarde)
{
	SPI_HandleTypeDef * const spi = ((BoardSPI::BoardSPI1==eh) ? &hspi1 : &hspi2);
	UInt8 * const uitPtr = reinterpret_cast<UInt8 *>(const_cast<UInt32 *>(&waarde));
	UInt32 retwaarde;

	if (BoardSPI::BoardSPI1==eh)
	{

#ifndef FreeRTOSGebruikt
		spi1Sem=false;
#endif
		UInt8 * const inPtr = reinterpret_cast<UInt8 *>(&retwaarde);
		/* spi 1 verstuurt met 1 bytes/eenheid */
		ONGEBRUIKT const auto status  = HAL_SPI_TransmitReceive_DMA(spi,
		                                                            uitPtr,
		                                                            inPtr,
		                                                            sizeof(UInt32));

		assert(HAL_OK == status);
	}
	else
	{
#ifndef FreeRTOSGebruikt
		spi2Sem=false;
#endif
		/* spi 2 verstuurt met 16 bytes/eenheid */
		ONGEBRUIKT const HAL_StatusTypeDef status  = HAL_SPI_Transmit_DMA(spi,
		                                                                  uitPtr,
		                                                                  sizeof(UInt32)/2);

		assert(HAL_OK == status);

		retwaarde = 0x1234;
	}

	spiSemWacht(eh);
	return(retwaarde);
}

void STM32BoardController::verstuur(const DataPakket<UInt32> &zendPakket,
                                    const DataPakket<UInt32> &ontvangPakket)
{
	auto * const spi =  &hspi1;

	UInt8 * const uitPtr = reinterpret_cast<UInt8 *>(const_cast<UInt32 *>(&(zendPakket[0])));
	UInt8 * const inPtr = reinterpret_cast<UInt8 *>(const_cast<UInt32 *>(&(ontvangPakket[0])));

#ifndef FreeRTOSGebruikt
	spi1Sem=false;
#endif

	ONGEBRUIKT const HAL_StatusTypeDef status  = HAL_SPI_TransmitReceive_DMA(spi,
	                                                                         uitPtr,
	                                                                         inPtr,
	                                                                         sizeof(UInt32)*zendPakket.geefGrootte());

	assert(HAL_OK == status);

	spiSemWacht(BoardSPI::BoardSPI1);

}

void STM32BoardController::verstuur(const STM32BoardController::BoardSPI eh,
                                    const DataPakket<UInt8> &zendPakket)
{
	auto * spi = ((BoardSPI::BoardSPI1==eh) ? &hspi1 : &hspi2);
	auto uitPtr = const_cast<UInt8 *>(&(zendPakket[0]));

#ifndef FreeRTOSGebruikt
	if (BoardSPI::BoardSPI1==eh)
		spi1Sem=false;
	else
		spi2Sem=false;
#endif

	ONGEBRUIKT const HAL_StatusTypeDef status  = HAL_SPI_Transmit_DMA(spi,
	                                                                  uitPtr,
	                                                                  zendPakket.geefGrootte());

	assert(HAL_OK == status);

	spiSemWacht(eh);
}

void STM32BoardController::verstuur(const STM32BoardController::BoardSPI eh,
                                    const DataPakket<UInt32> &zendPakket)
{
	SPI_HandleTypeDef * const spi = ((BoardSPI::BoardSPI1==eh) ? &hspi1 : &hspi2);
	UInt8 * const uitPtr = reinterpret_cast<UInt8 *>(const_cast<UInt32 *>(&(zendPakket[0])));

#ifndef FreeRTOSGebruikt
	if (BoardSPI::BoardSPI1==eh)
		spi1Sem=false;
	else
		spi2Sem=false;
#endif

	spiSemWacht(eh);

	ONGEBRUIKT const HAL_StatusTypeDef status = HAL_SPI_Transmit_DMA(spi,
	                                                                 uitPtr,
	                                                                 sizeof(UInt32)*zendPakket.geefGrootte());

	assert(HAL_OK == status);

}

void STM32BoardController::spiSemWacht(const STM32BoardController::BoardSPI eh)
{
	if (BoardSPI::BoardSPI1 == eh)
	{
#ifndef FreeRTOSGebruikt
		while (false == spi1Sem);
#else
		spi1Sem.Take();
#endif
	} else
	{
#ifndef FreeRTOSGebruikt
		while (false == spi2Sem);
#else
		spi2Sem.Take();
#endif
	}
}

void STM32BoardController::verstuur(const STM32BoardController::BoardUART eh,
                                    const DataPakket<UInt8> &zendPakket)
{
	auto * const huart = ((BoardUART::BoardUART2 == eh) ? &huart2 : &huart6);
	auto * const tekens = zendPakket.geefPtr();

	ONGEBRUIKT const HAL_StatusTypeDef status  = HAL_UART_Transmit_DMA(huart,tekens,zendPakket.geefGrootte());
	assert(HAL_OK == status);

#ifndef FreeRTOSGebruikt
	if (BoardUART::BoardUART2==eh)
		usart2Sem[sem(Semafoor::Tx)]=false;
	else
		usart6Sem[sem(Semafoor::Tx)]=false;
#endif

	if (BoardUART::BoardUART2==eh)
	{

#ifndef FreeRTOSGebruikt
		while (false == usart2Sem[sem(Semafoor::Tx)]);
#else
		usart2Sem[sem(Semafoor::Tx)].Take();
#endif
	}
	else
	{
#ifndef FreeRTOSGebruikt
		while (false == usart6Sem[sem(Semafoor::Tx)]);
#else
		usart6Sem[sem(Semafoor::Tx)].Take();
#endif
	}
}

void STM32BoardController::verstuurByte(const STM32BoardController::BoardUART eh,
                                        const UInt8 data)
{
	auto * const huart = ((BoardUART::BoardUART2 == eh) ? &huart2 : &huart6);

#ifndef FreeRTOSGebruikt
	if (BoardUART::BoardUART2==eh)
		usart2Sem[sem(Semafoor::Tx)]=false;
	else
		usart6Sem[sem(Semafoor::Tx)]=false;
#endif

	ONGEBRUIKT const HAL_StatusTypeDef status  = HAL_UART_Transmit_IT(huart, const_cast<UInt8 *>(&data),1);
	assert(HAL_OK == status);

	if (BoardUART::BoardUART2==eh)
	{
#ifndef FreeRTOSGebruikt
		while (false == usart2Sem[sem(Semafoor::Tx)]);
#else
		usart2Sem[sem(Semafoor::Tx)].Take();
#endif
	}
	else
	{
#ifndef FreeRTOSGebruikt
		while (false == usart6Sem[sem(Semafoor::Tx)]);
#else
		usart6Sem[sem(Semafoor::Tx)].Take();
#endif
	}
}

UInt8 STM32BoardController::ontvangByte(const STM32BoardController::BoardUART eh)
{
	auto * const huart = ((BoardUART::BoardUART2 == eh) ? &huart2 : &huart6);
	UInt8 data;

#ifndef FreeRTOSGebruikt
	if (BoardUART::BoardUART2==eh)
		usart2Sem[sem(Semafoor::Rx)]=false;
	else
		usart6Sem[sem(Semafoor::Rx)]=false;
#else

#endif
	ONGEBRUIKT const HAL_StatusTypeDef status  = HAL_UART_Receive_IT(huart,&data,1);

	assert(HAL_OK == status);

	if (BoardUART::BoardUART2==eh)
	{
#ifndef FreeRTOSGebruikt
		while (false == usart2Sem[sem(Semafoor::Rx)]);
#else
		usart2Sem[sem(Semafoor::Rx)].Take();
#endif
	}
	else
	{
#ifndef FreeRTOSGebruikt
		while (false == usart6Sem[sem(Semafoor::Rx)]);
#else
		usart6Sem[sem(Semafoor::Rx)].Take();
#endif
	}

	return(data);
}

void STM32BoardController::ontvang(const STM32BoardController::BoardUART eh,
                                   const DataPakket<UInt8> &ontvangPakket)
{
	auto * huart = ((BoardUART::BoardUART2 == eh) ? &huart2 : &huart6);
	auto tekens = reinterpret_cast<UInt8 *>(&(ontvangPakket[0]));

#ifndef FreeRTOSGebruikt
	if (BoardUART::BoardUART2==eh)
		usart2Sem[sem(Semafoor::Rx)]=false;
	else
		usart6Sem[sem(Semafoor::Rx)]=false;
#endif

	ONGEBRUIKT const HAL_StatusTypeDef status = HAL_UART_Receive_DMA(huart,tekens,ontvangPakket.geefGrootte());

	assert(HAL_OK == status);

	if (BoardUART::BoardUART2==eh)
	{
#ifndef FreeRTOSGebruikt
		while (false == usart2Sem[sem(Semafoor::Rx)]);
#else
		usart2Sem[sem(Semafoor::Rx)].Take();
#endif
	}
	else
	{
#ifndef FreeRTOSGebruikt
		while (false == usart6Sem[sem(Semafoor::Rx)]);
#else
		usart6Sem[sem(Semafoor::Rx)].Take();
#endif
	}

}

void STM32BoardController::verstuur(const BoardI2C eh,
                                    const DataPakket<UInt8> &zendPakket,
                                    const UInt8 slaafAdres)
{
	static const IRQn_Type i2cEVirqn[] = { I2C1_EV_IRQn , I2C2_EV_IRQn };
	static const IRQn_Type i2cERirqn[] = { I2C1_ER_IRQn , I2C2_ER_IRQn };
	static const IRQn_Type i2cDMAirqn[] = { DMA1_Stream1_IRQn , DMA1_Stream7_IRQn };

	assert(eh < BoardI2C::LaatsteI2C);

	I2C_HandleTypeDef * const i2c = ((BoardI2C::BoardI2C1==eh) ? &hi2c1 : &hi2c2);
#ifndef FreeRTOSGebruikt
	volatile bool &dezesem = ((BoardI2C::BoardI2C1==eh) ?
	                          i2c1Sem[sem(Semafoor::Tx)] :
	                          i2c2Sem[sem(Semafoor::Tx)]);
#else
	CountingSemaphore &dezesem = ((BoardI2C::BoardI2C1==eh) ?
	                              i2c1Sem[sem(Semafoor::Tx)] : i2c2Sem[sem(Semafoor::Tx)]);
#endif

	UInt8 * const uitPtr = &(zendPakket[0]);
	const UInt8 slaaf8bitAdres = (slaafAdres<<1);
	const Teller masterIndex = static_cast<Teller>(eh);

	/* HAL I2C DMA toepassing vereist zowel i2c irq (voor sturen van dev adres) als dma irq ( voor sturen van data) */
	NVIC_EnableIRQ(i2cEVirqn[masterIndex]);
	NVIC_EnableIRQ(i2cERirqn[masterIndex]);
	NVIC_EnableIRQ(i2cDMAirqn[masterIndex]);
#ifndef FreeRTOSGebruikt
	dezesem = false;
#endif
	while (HAL_OK != HAL_I2C_Master_Transmit_DMA(i2c,
	                                             slaaf8bitAdres,
	                                             uitPtr,
	                                             zendPakket.geefGrootte()))
	{
		/* Error_Handler() function is called when Timeout error occurs.
			   When Acknowledge failure occurs (Slave don't acknowledge its address)
			   Master restarts communication */
		if (HAL_I2C_GetError(i2c) != HAL_I2C_ERROR_NONE)
			StopHier();
	}

#ifndef FreeRTOSGebruikt
	while (false == dezesem);
#else
	dezesem.Take();
#endif

	NVIC_DisableIRQ(i2cEVirqn[masterIndex]);
	NVIC_DisableIRQ(i2cERirqn[masterIndex]);
	NVIC_DisableIRQ(i2cDMAirqn[masterIndex]);
}

void STM32BoardController::ontvang(const BoardI2C eh,
                                   const DataPakket<UInt8> &ontvangPakket,
                                   const UInt8 slaafAdres)
{
	static const IRQn_Type i2cEVirqn[] = { I2C1_EV_IRQn , I2C2_EV_IRQn };
	static const IRQn_Type i2cERirqn[] = { I2C1_ER_IRQn , I2C2_ER_IRQn };
	static const IRQn_Type i2cDMAirqn[] = { DMA1_Stream0_IRQn , DMA1_Stream2_IRQn };

	assert(eh < BoardI2C::LaatsteI2C);

	I2C_HandleTypeDef * const i2c = ((BoardI2C::BoardI2C1==eh) ? &hi2c1 : &hi2c2);
#ifndef FreeRTOSGebruikt
	volatile bool &dezesem = ((BoardI2C::BoardI2C1==eh) ?
	                          i2c1Sem[sem(Semafoor::Rx)] :
	                          i2c2Sem[sem(Semafoor::Rx)]);
#else
	CountingSemaphore &dezesem = ((BoardI2C::BoardI2C1==eh) ?
	                              i2c1Sem[sem(Semafoor::Rx)] : i2c2Sem[sem(Semafoor::Rx)]);
#endif

	UInt8 * const inPtr = &(ontvangPakket[0]);
	const UInt8 slaaf8bitAdres = (slaafAdres<<1);
	const auto masterIndex = static_cast<Teller>(eh);

	/* HAL I2C DMA toepassing vereist zowel i2c irq (voor sturen van dev adres) als dma irq ( voor sturen van data) */
	NVIC_EnableIRQ(i2cEVirqn[masterIndex]);
	NVIC_EnableIRQ(i2cERirqn[masterIndex]);
	NVIC_EnableIRQ(i2cDMAirqn[masterIndex]);

#ifndef FreeRTOSGebruikt
	dezesem = false;
#endif

	while (HAL_OK != HAL_I2C_Master_Receive_DMA(i2c,
	                                            slaaf8bitAdres,
	                                            inPtr,
	                                            ontvangPakket.geefGrootte()))
	{
		/* Error_Handler() function is called when Timeout error occurs.
			   When Acknowledge failure occurs (Slave don't acknowledge its address)
			   Master restarts communication */
		if (HAL_I2C_GetError(i2c) != HAL_I2C_ERROR_NONE)
			StopHier();
	}

#ifndef FreeRTOSGebruikt
	while (false == dezesem);
#else
	dezesem.Take();
#endif

	NVIC_DisableIRQ(i2cEVirqn[masterIndex]);
	NVIC_DisableIRQ(i2cERirqn[masterIndex]);
	NVIC_DisableIRQ(i2cDMAirqn[masterIndex]);
}


void STM32BoardController::selekteerInput(const STM32BoardController::DSBSignaalSelektor sel) const
{
	/* DSB Signaal Selektor0 op PF10
	* DSB Signaal Selektor1 op PF13 :
	 * 00 ==> Standaard : BNC connector.
	 * 01 ==> Lokaal DAC signaal (DAC2)
	 * 11 ==> Aux signaal (header) */

	static const GPIO_PinState pf10[] = { GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_SET};
	static const GPIO_PinState pf13[] = { GPIO_PIN_RESET, GPIO_PIN_RESET , GPIO_PIN_SET };
	const auto index = static_cast<UInt32>(sel);

	const GPIO_PinState dsbctrl0 = pf10[index];
	const GPIO_PinState dsbctrl1 = pf13[index];

	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_10,dsbctrl0);
	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_13,dsbctrl1);

}

void STM32BoardController::selekteerProces(const STM32BoardController::RGTProcesSelektor sel) const
{
	/* RGTCtrl[0..1] op PD12 en PD13 */
	/* 00 ==> eerste orde proces
	 * 10 ==> tweede orde proces
	 * 01 ==> digitaal proces */

	static const GPIO_PinState pd12[] = { GPIO_PIN_RESET, GPIO_PIN_SET,GPIO_PIN_RESET };
	static const GPIO_PinState pd13[] = { GPIO_PIN_RESET, GPIO_PIN_RESET,GPIO_PIN_SET };
	const auto index = static_cast<UInt32>(sel);

	const GPIO_PinState rgtctrl0 = pd12[index];
	const GPIO_PinState rgtctrl1 = pd13[index];

	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,rgtctrl0);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,rgtctrl1);
}

void STM32BoardController::selekteerADCAntiAlias(const STM32BoardController::AntiAliasCtrl sel) const
{
	/* ADC Anti Alias Ctrl :
	 * ADC op PE5
	 */
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,((sel == AntiAliasCtrl::AA50kHz) ? GPIO_PIN_RESET : GPIO_PIN_SET));
}

void STM32BoardController::selekteerDACAntiAlias(const STM32BoardController::AntiAliasCtrl sel)
{
	/*  DAC Anti Alias Ctrl :
	 * DAC op PE6
	 * */
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,((sel == AntiAliasCtrl::AA50kHz) ? GPIO_PIN_RESET : GPIO_PIN_SET));
}


#define uSD_D0_Pin GPIO_PIN_8
#define uSD_D1_Pin GPIO_PIN_9
#define uSD_D2_Pin GPIO_PIN_10
#define uSD_D3_Pin GPIO_PIN_11
#define uSD_CLK_Pin GPIO_PIN_12
#define uSD_CMD_Pin GPIO_PIN_2
#define uSD_CMD_GPIO_Port GPIOD
#define I2C1_SCL_Pin GPIO_PIN_6
#define I2C1_SDA_Pin GPIO_PIN_7

void STM32BoardController::gpioInit()
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* Peripheral clock enable */
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();

	/* RGTCtrl[0..1] op PD12 en PD13 */
	GPIO_InitStruct.Pin = GPIO_PIN_12+GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/* ADC en DAC Anti Alias Ctrl :
	 * ADC op PE5
	 * DAC op PE6
	 * */
	GPIO_InitStruct.Pin = GPIO_PIN_5+GPIO_PIN_6;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/* DigProces[3..4] en DSBSignaalSelektor[1..0] :
	 * DigProces3 op PF2
	 * DigProces4 op PF3
	 * DSB Signaal Selektor0 op PF10
	 * DSB Signaal Selektor1 op PF13
	 */
	GPIO_InitStruct.Pin = GPIO_PIN_2+GPIO_PIN_3+GPIO_PIN_10+GPIO_PIN_13   ;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	/* DigProces[0..2]  :
	 * DigProces0 op PG10
	 * DigProces1 op PG11
	 * DigProces2 op PG14
	 */
	GPIO_InitStruct.Pin = GPIO_PIN_10+GPIO_PIN_11+GPIO_PIN_14;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

}

void STM32BoardController::spiInit()
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* Peripheral clock enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_SPI1_CLK_ENABLE();


	/**SPI1 GPIO Configuration
	PA4     ------> SPI1_NSS
	PA5     ------> SPI1_SCK
	PA6     ------> SPI1_MISO
	PA7     ------> SPI1_MOSI
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_5+GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	hspi1.Instance = SPI1;
	auto &spi1Init = hspi1.Init;
	spi1Init.Mode = SPI_MODE_MASTER;
	spi1Init.Direction = SPI_DIRECTION_2LINES;
	spi1Init.DataSize = SPI_DATASIZE_8BIT;
	spi1Init.CLKPolarity = SPI_POLARITY_LOW; /* CPOL = 0 */
	spi1Init.CLKPhase = SPI_PHASE_2EDGE;     /* CPHA = 1 */
	spi1Init.NSS = SPI_NSS_HARD_OUTPUT;
	spi1Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4; /* 96/(4) = 24 : Max 25 Mhz voor ADS131A02  */
	spi1Init.FirstBit = SPI_FIRSTBIT_MSB;
	spi1Init.TIMode = SPI_TIMODE_DISABLE;
	spi1Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	spi1Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi1) != HAL_OK)
	{
		StopHier();
	}
	else
	{
		__HAL_RCC_DMA2_CLK_ENABLE();

		/* Peripheral DMA start*/
		hdma_spi1_tx.Instance = DMA2_Stream2;
		auto &txInit = hdma_spi1_tx.Init;
		txInit.Channel = DMA_CHANNEL_2;
		txInit.Direction = DMA_MEMORY_TO_PERIPH;
		txInit.PeriphInc = DMA_PINC_DISABLE;
		txInit.MemInc = DMA_MINC_ENABLE;
		txInit.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		txInit.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		txInit.Mode = DMA_NORMAL;
		txInit.Priority = DMA_PRIORITY_HIGH;
		txInit.FIFOMode = DMA_FIFOMODE_ENABLE;
		txInit.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
		txInit.MemBurst = DMA_MBURST_SINGLE;
		txInit.PeriphBurst = DMA_PBURST_SINGLE;
		if (HAL_DMA_Init(&hdma_spi1_tx) != HAL_OK)
			StopHier();
		else
		{
			HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 1, 0);
			HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
			__HAL_LINKDMA(&hspi1, hdmatx, hdma_spi1_tx);
		}
		hdma_spi1_rx.Instance = DMA2_Stream0;
		auto &rxInit = hdma_spi1_rx.Init;
		rxInit.Channel = DMA_CHANNEL_3;
		rxInit.Direction = DMA_PERIPH_TO_MEMORY;
		rxInit.PeriphInc = DMA_PINC_DISABLE;
		rxInit.MemInc = DMA_MINC_ENABLE;
		rxInit.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		rxInit.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		rxInit.Mode = DMA_NORMAL;
		rxInit.Priority = DMA_PRIORITY_VERY_HIGH;
		rxInit.FIFOMode = DMA_FIFOMODE_ENABLE;
		rxInit.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
		rxInit.MemBurst = DMA_MBURST_SINGLE;
		rxInit.PeriphBurst = DMA_PBURST_SINGLE;;
		if (HAL_DMA_Init(&hdma_spi1_rx) != HAL_OK)
			StopHier();
		else
		{
			HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 1, 0);
			HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
			__HAL_LINKDMA(&hspi1, hdmarx, hdma_spi1_rx);

			/* Peripheral interrupt start */
			HAL_NVIC_SetPriority(SPI1_IRQn, 2, 0);
			HAL_NVIC_EnableIRQ(SPI1_IRQn);
		}
	}

	/* Peripheral clock enable */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_SPI2_CLK_ENABLE();

	/**SPI2 GPIO Configuration
	PC3     ------> SPI2_MOSI
	PB12     ------> SPI2_NSS
	PB13     ------> SPI2_SCK
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	hspi2.Instance = SPI2;
	auto &spi2Init = hspi2.Init;
	spi2Init.Mode = SPI_MODE_MASTER;
	spi2Init.Direction = SPI_DIRECTION_1LINE;
	spi2Init.DataSize = SPI_DATASIZE_8BIT;
	spi2Init.CLKPolarity = SPI_POLARITY_HIGH;
	spi2Init.CLKPhase = SPI_PHASE_1EDGE;
	spi2Init.NSS = SPI_NSS_HARD_OUTPUT;
	spi2Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;  /* 96/4 = 24 : max 30 MHz voor MAX5136 */
	spi2Init.FirstBit = SPI_FIRSTBIT_MSB;
	spi2Init.TIMode = SPI_TIMODE_DISABLE;
	spi2Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	spi2Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi2) != HAL_OK)
	{
		StopHier();
	}
	else
	{
		__HAL_RCC_DMA1_CLK_ENABLE();
		/* Peripheral DMA start*/
		hdma_spi2_tx.Instance = DMA1_Stream4;
		auto &txInit = hdma_spi2_tx.Init;
		txInit.Channel = DMA_CHANNEL_0;
		txInit.Direction = DMA_MEMORY_TO_PERIPH;
		txInit.PeriphInc = DMA_PINC_DISABLE;
		txInit.MemInc = DMA_MINC_ENABLE;
		txInit.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		txInit.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		txInit.Mode = DMA_NORMAL;
		txInit.Priority = DMA_PRIORITY_HIGH;
		txInit.FIFOMode =  DMA_FIFOMODE_DISABLE; //DMA_FIFOMODE_ENABLE;
		txInit.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
		txInit.MemBurst = DMA_MBURST_SINGLE;
		txInit.PeriphBurst = DMA_PBURST_SINGLE;

		if (HAL_DMA_Init(&hdma_spi2_tx) != HAL_OK)
			StopHier();
		else
		{
			__HAL_LINKDMA(&hspi2, hdmatx, hdma_spi2_tx);
			HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 1, 0);
			HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
		}

		/* Peripheral interrupt start */
		HAL_NVIC_SetPriority(SPI2_IRQn, 2, 0);
		HAL_NVIC_EnableIRQ(SPI2_IRQn);
	}
}

void STM32BoardController::i2cInit()
{

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* Peripheral clock enable */
	__HAL_RCC_I2C1_CLK_ENABLE();
	__HAL_RCC_I2C2_CLK_ENABLE();

	__HAL_RCC_GPIOB_CLK_ENABLE();

	/** I2C1 GPIO Configuration
	PB6     ------> I2C1_SCL
	PB7     ------> I2C1_SDA
	*/
	GPIO_InitStruct.Pin = I2C1_SCL_Pin|I2C1_SDA_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 400000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = EigenI2CAdres;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK)
		StopHier();
	else
	{
		__HAL_RCC_DMA1_CLK_ENABLE();

		/* Peripheral DMA init*/

		hdma_i2c1_rx.Instance = DMA1_Stream0;
		hdma_i2c1_rx.Init.Channel = DMA_CHANNEL_1;
		hdma_i2c1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_i2c1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_i2c1_rx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_i2c1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_i2c1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_i2c1_rx.Init.Mode = DMA_NORMAL;
		hdma_i2c1_rx.Init.Priority = DMA_PRIORITY_MEDIUM;
		hdma_i2c1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		if (HAL_DMA_Init(&hdma_i2c1_rx) != HAL_OK)
			StopHier();
		else
		{
			__HAL_LINKDMA(&hi2c1, hdmarx, hdma_i2c1_rx);
			HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 12, 0);
		}

		hdma_i2c1_tx.Instance = DMA1_Stream1;
		hdma_i2c1_tx.Init.Channel = DMA_CHANNEL_0;
		hdma_i2c1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma_i2c1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_i2c1_tx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_i2c1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_i2c1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_i2c1_tx.Init.Mode = DMA_NORMAL;
		hdma_i2c1_tx.Init.Priority = DMA_PRIORITY_LOW;
		hdma_i2c1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		if (HAL_DMA_Init(&hdma_i2c1_tx) != HAL_OK)
			StopHier();
		else
		{
			__HAL_LINKDMA(&hi2c1,hdmatx,hdma_i2c1_tx);
			HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 13, 0);
		}
	}

	/**I2C2 GPIO Configuration
    PB10     ------> I2C2_SCL
    PB9     ------> I2C2_SDA
    */
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_9;
	GPIO_InitStruct.Alternate = GPIO_AF9_I2C2;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	hi2c2.Instance = I2C2;
	hi2c2.Init.ClockSpeed = 400000;
	hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c2.Init.OwnAddress1 = EigenI2CAdres;
	hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c2.Init.OwnAddress2 = 0;
	hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c2) != HAL_OK)
		StopHier();
	else
	{
		__HAL_RCC_DMA1_CLK_ENABLE();

		/* Peripheral DMA init*/

		hdma_i2c2_rx.Instance = DMA1_Stream2;
		hdma_i2c2_rx.Init.Channel = DMA_CHANNEL_7;
		hdma_i2c2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_i2c2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_i2c2_rx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_i2c2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_i2c2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_i2c2_rx.Init.Mode = DMA_NORMAL;
		hdma_i2c2_rx.Init.Priority = DMA_PRIORITY_MEDIUM;
		hdma_i2c2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		if (HAL_DMA_Init(&hdma_i2c2_rx) != HAL_OK)
			StopHier();
		else
		{
			__HAL_LINKDMA(&hi2c2, hdmarx, hdma_i2c2_rx);
			HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 12, 0);
		}

		hdma_i2c2_tx.Instance = DMA1_Stream7;
		hdma_i2c2_tx.Init.Channel = DMA_CHANNEL_7;
		hdma_i2c2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma_i2c2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_i2c2_tx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_i2c2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_i2c2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_i2c2_tx.Init.Mode = DMA_NORMAL;
		hdma_i2c2_tx.Init.Priority = DMA_PRIORITY_LOW;
		hdma_i2c2_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		if (HAL_DMA_Init(&hdma_i2c2_tx) != HAL_OK)
			StopHier();
		else
		{
			__HAL_LINKDMA(&hi2c2,hdmatx,hdma_i2c2_tx);
			HAL_NVIC_SetPriority(DMA1_Stream7_IRQn, 12, 0);
		}
	}
}

#define STLINK_RX_Pin GPIO_PIN_2
#define STLINK_TX_Pin GPIO_PIN_3

void STM32BoardController::uart2Init()
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* Peripheral clock enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_USART2_CLK_ENABLE();


	/**USART2 GPIO Configuration
	PA2     ------> USART2_TX
	PA3     ------> USART2_RX
	*/
	GPIO_InitStruct.Pin = STLINK_RX_Pin | STLINK_TX_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart2) != HAL_OK)
		StopHier();
	else
	{
		__HAL_RCC_DMA1_CLK_ENABLE();
		/* Peripheral DMA init*/

		hdma_usart2_rx.Instance = DMA1_Stream5;
		hdma_usart2_rx.Init.Channel = DMA_CHANNEL_4;
		hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_usart2_rx.Init.Mode = DMA_NORMAL;
		hdma_usart2_rx.Init.Priority = DMA_PRIORITY_LOW;
		hdma_usart2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
			StopHier();
		else
		{
			__HAL_LINKDMA(&huart2, hdmarx, hdma_usart2_rx);

			/* DMA1_Stream5_IRQn interrupt configuration */
			HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 13, 0);
			HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

			hdma_usart2_tx.Instance = DMA1_Stream6;
			hdma_usart2_tx.Init.Channel = DMA_CHANNEL_4;
			hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
			hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
			hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;
			hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
			hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
			hdma_usart2_tx.Init.Mode = DMA_NORMAL;
			hdma_usart2_tx.Init.Priority = DMA_PRIORITY_LOW;
			hdma_usart2_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
			if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK)
				StopHier();
			else
			{
				__HAL_LINKDMA(&huart2, hdmatx, hdma_usart2_tx);

				/* DMA1_Stream6_IRQn interrupt configuration */
				HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 13, 0);
				HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
			}
		}
	}
}

void STM32BoardController::usart6Init()
{
	GPIO_InitTypeDef GPIO_InitStruct;

	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_USART6_CLK_ENABLE();

	/**USART6 GPIO Configuration
	PC6     ------> USART6_TX
	PG9     ------> USART6_RX
	PG12     ------> USART6_RTS
	PG13     ------> USART6_CTS
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	huart6.Instance = USART6;
#ifdef	BoardVersie10
	huart6.Init.BaudRate = 115200;
	huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE; // Prototype heeft rts en cts omgedraaid, dus hier geen hw control
	huart6.Init.OverSampling = UART_OVERSAMPLING_16;
#else
	huart6.Init.BaudRate = 2000000; //921600; /* dezelfde waarde als in desktop applikatie. */
	huart6.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
	huart6.Init.OverSampling = UART_OVERSAMPLING_8;
#endif

	huart6.Init.WordLength = UART_WORDLENGTH_8B;
	huart6.Init.StopBits = UART_STOPBITS_1;
	huart6.Init.Parity = UART_PARITY_NONE;
	huart6.Init.Mode = UART_MODE_TX_RX;

	if (HAL_UART_Init(&huart6) != HAL_OK)
		StopHier();
	else
	{
		__HAL_RCC_DMA2_CLK_ENABLE();
		/* Peripheral DMA init*/

		hdma_usart6_rx.Instance = DMA2_Stream1;
		hdma_usart6_rx.Init.Channel = DMA_CHANNEL_5;
		hdma_usart6_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_usart6_rx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_usart6_rx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_usart6_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_usart6_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_usart6_rx.Init.Mode = DMA_NORMAL;
		hdma_usart6_rx.Init.Priority = DMA_PRIORITY_HIGH;
		hdma_usart6_rx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
		hdma_usart6_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_3QUARTERSFULL;
		hdma_usart6_rx.Init.MemBurst = DMA_MBURST_SINGLE;
		hdma_usart6_rx.Init.PeriphBurst = DMA_PBURST_SINGLE;

		if (HAL_DMA_Init(&hdma_usart6_rx) != HAL_OK)
			StopHier();
		else
		{
			__HAL_LINKDMA(&huart6, hdmarx, hdma_usart6_rx);
			/* DMA2_Stream1_IRQn interrupt configuration */
			HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 6, 0);
			HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

			hdma_usart6_tx.Instance = DMA2_Stream7;
			hdma_usart6_tx.Init.Channel = DMA_CHANNEL_5;
			hdma_usart6_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
			hdma_usart6_tx.Init.PeriphInc = DMA_PINC_DISABLE;
			hdma_usart6_tx.Init.MemInc = DMA_MINC_ENABLE;
			hdma_usart6_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
			hdma_usart6_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
			hdma_usart6_tx.Init.Mode = DMA_NORMAL;
			hdma_usart6_tx.Init.Priority = DMA_PRIORITY_MEDIUM;
			hdma_usart6_tx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
			hdma_usart6_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_3QUARTERSFULL;
			hdma_usart6_tx.Init.MemBurst = DMA_MBURST_SINGLE;
			hdma_usart6_tx.Init.PeriphBurst = DMA_PBURST_SINGLE;
			if (HAL_DMA_Init(&hdma_usart6_tx) != HAL_OK)
				StopHier();
			else
			{
				__HAL_LINKDMA(&huart6, hdmatx, hdma_usart6_tx);

				/* DMA2_Stream7_IRQn interrupt configuration */
				HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 6, 0);
				HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);

				HAL_NVIC_SetPriority(USART6_IRQn, 10, 0);
			}
		}
	}
}


UART_HandleTypeDef STM32BoardController::huart6;
DMA_HandleTypeDef STM32BoardController::hdma_usart6_tx;
DMA_HandleTypeDef STM32BoardController::hdma_usart6_rx;

DMA_HandleTypeDef STM32BoardController::hdma_spi1_tx;
DMA_HandleTypeDef STM32BoardController::hdma_spi1_rx;

UART_HandleTypeDef STM32BoardController::huart2;
DMA_HandleTypeDef STM32BoardController::hdma_usart2_tx;
DMA_HandleTypeDef STM32BoardController::hdma_usart2_rx;

DMA_HandleTypeDef STM32BoardController::hdma_spi2_tx;

DMA_HandleTypeDef STM32BoardController::hdma_i2c1_tx;
DMA_HandleTypeDef STM32BoardController::hdma_i2c1_rx;
DMA_HandleTypeDef STM32BoardController::hdma_i2c2_tx;
DMA_HandleTypeDef STM32BoardController::hdma_i2c2_rx;
I2C_HandleTypeDef STM32BoardController::hi2c1;
I2C_HandleTypeDef STM32BoardController::hi2c2;


extern "C" void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	static constexpr auto spi1= &STM32BoardController::hspi1;
	static constexpr auto spi2= &STM32BoardController::hspi2;

	LL_SPI_Disable(hspi->Instance);

	if (spi1 == hspi)
	{
#ifndef FreeRTOSGebruikt
		STM32BoardController::spi1Sem = true;
#else
		auto &sem = STM32BoardController::spi1Sem;
		STM32BoardController::isrSemafoorAfhandeling(sem);
#endif
	}
	else
	if (spi2 == hspi)
	{
#ifndef FreeRTOSGebruikt
		STM32BoardController::spi2Sem = true;
#else
		auto &sem = STM32BoardController::spi2Sem;
		STM32BoardController::isrSemafoorAfhandeling(sem);
#endif
	}
}

extern "C" void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	static constexpr auto spi1= &STM32BoardController::hspi1;

	if (spi1 == hspi)
	{
/* schakel de spi uit om de nCS lijn weer hoog te brengen. */
		__HAL_SPI_DISABLE(&(STM32BoardController::hspi1));
#ifndef FreeRTOSGebruikt
		STM32BoardController::spi1Sem = true;
#else
		CountingSemaphore &sem = STM32BoardController::spi1Sem;
		STM32BoardController::isrSemafoorAfhandeling(sem);
#endif
	}
}

/* C++ name mangling mag niet de interrupt handler namen veranderen, 
 * deze staan in een assembly bestand zo genoemd.
 * Derhalve hieronder met C-Linking compileren.
 */
#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief This function handles I2C1 event interrupt.
*/
void I2C1_EV_IRQHandler(void)
{
	static constexpr auto perif = &STM32BoardController::hi2c1;
	HAL_I2C_EV_IRQHandler(perif);
}


void I2C1_ER_IRQHandler(void)
{
	static constexpr auto perif = &STM32BoardController::hi2c1;
	HAL_I2C_ER_IRQHandler(perif);
}

/**
* @brief This function handles I2C1 event interrupt.
*/
void I2C2_EV_IRQHandler(void)
{
	static constexpr auto perif = &STM32BoardController::hi2c2;
	HAL_I2C_EV_IRQHandler(perif);
}


void I2C2_ER_IRQHandler(void)
{
	static constexpr auto perif = &STM32BoardController::hi2c2;
	HAL_I2C_ER_IRQHandler(perif);
}


/**
* @brief This function handles SPI1 global interrupt.
*/
void SPI1_IRQHandler(void)
{
	static constexpr auto perif = &STM32BoardController::hspi1;
	HAL_SPI_IRQHandler(perif);
}

/**
* @brief This function handles SPI2 global interrupt.
*/
void SPI2_IRQHandler(void)
{
	static constexpr auto perif = &STM32BoardController::hspi2;
	HAL_SPI_IRQHandler(perif);
}

void USART2_IRQHandler()
{
	HAL_UART_IRQHandler(&(STM32BoardController::huart2));
	static constexpr auto index = STM32BoardController::sem(STM32BoardController::Semafoor::Rx);
#ifndef FreeRTOSGebruikt
	STM32BoardController::usart2Sem[index] = true;
#else
	CountingSemaphore &sem = STM32BoardController::usart2Sem[index];
	STM32412gDiscovery::isrSemafoorAfhandeling(sem);
#endif
}

void USART6_IRQHandler()
{
	const bool rxInterrupt = (HAL_UART_STATE_BUSY_RX  == STM32BoardController::huart6.RxState);
	const bool txInterrupt = (HAL_UART_STATE_BUSY_TX  == STM32BoardController::huart6.gState);

	HAL_UART_IRQHandler(&(STM32BoardController::huart6));

	if (true == rxInterrupt)
	{
		const bool rxEindeInterrupt = (HAL_UART_STATE_READY  == STM32BoardController::huart6.RxState);
		if (true == rxEindeInterrupt)
		{
			static constexpr auto index = STM32BoardController::sem(STM32BoardController::Semafoor::Rx);
#ifndef FreeRTOSGebruikt
			STM32BoardController::usart6Sem[index] = true;
#else
			auto &sem = STM32BoardController::usart6Sem[index];
			STM32BoardController::isrSemafoorAfhandeling(sem);
#endif
		}
	}
	else if (true == txInterrupt)
	{
		const bool txEindeInterrupt = (HAL_UART_STATE_READY  == STM32BoardController::huart6.gState);
		if (true == txEindeInterrupt)
		{
			static constexpr auto index = STM32BoardController::sem(STM32BoardController::Semafoor::Tx);
#ifndef FreeRTOSGebruikt

			STM32BoardController::usart6Sem[index] = true;
#else
			auto &sem = STM32BoardController::usart6Sem[index];
			STM32BoardController::isrSemafoorAfhandeling(sem);
#endif
		}
	}
}

/**
* @brief This function handles DMA1 stream0 global interrupt.
*/
void DMA1_Stream0_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&STM32BoardController::hdma_i2c1_rx);
	if(HAL_DMA_STATE_READY == STM32BoardController::hdma_i2c1_rx.State)
	{
		static constexpr auto index = STM32BoardController::sem(STM32BoardController::Semafoor::Rx);
#ifndef FreeRTOSGebruikt
		STM32BoardController::i2c1Sem[index] = true;
#else
		auto &sem = STM32BoardController::i2c1Sem[index];
		STM32BoardController::isrSemafoorAfhandeling(sem);
#endif
	}

}

/**
* @brief This function handles DMA1 stream1 global interrupt.
*/
void DMA1_Stream1_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&STM32BoardController::hdma_i2c1_tx);
	if(HAL_DMA_STATE_READY == STM32BoardController::hdma_i2c1_tx.State)
	{
		static constexpr auto index = STM32BoardController::sem(STM32BoardController::Semafoor::Tx);
#ifndef FreeRTOSGebruikt
		STM32BoardController::i2c1Sem[index] = true;
#else
		auto &sem = STM32BoardController::i2c1Sem[index];
		STM32BoardController::isrSemafoorAfhandeling(sem);
#endif
	}
}

/**
* @brief This function handles DMA1 stream2 global interrupt.
*/
void DMA1_Stream2_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&STM32BoardController::hdma_i2c2_rx);
	if(HAL_DMA_STATE_READY == STM32BoardController::hdma_i2c2_rx.State)
	{
		static constexpr auto index = STM32BoardController::sem(STM32BoardController::Semafoor::Rx);
#ifndef FreeRTOSGebruikt
		STM32BoardController::i2c2Sem[index] = true;
#else
		auto &sem = STM32BoardController::i2c2Sem[index];
		STM32BoardController::isrSemafoorAfhandeling(sem);
#endif
	}

}

/**
* @brief This function handles DMA1 stream4 global interrupt.
*/
void DMA1_Stream4_IRQHandler(void)
{
	static constexpr auto perif = &(STM32BoardController::hdma_spi2_tx);
	HAL_DMA_IRQHandler(perif);
}

/**
* @brief This function handles DMA1 stream5 global interrupt.
*/
void DMA1_Stream5_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&STM32BoardController::hdma_usart2_rx);
	if(HAL_DMA_STATE_READY == STM32BoardController::hdma_usart2_rx.State)
	{
		static constexpr auto index = STM32BoardController::sem(STM32BoardController::Semafoor::Rx);
#ifndef FreeRTOSGebruikt
		STM32BoardController::usart2Sem[index] = true;
#else
		auto &sem = STM32BoardController::usart2Sem[index];
		STM32BoardController::isrSemafoorAfhandeling(sem);
#endif
	}
}

void DMA1_Stream7_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&STM32BoardController::hdma_i2c2_tx);
	if(HAL_DMA_STATE_READY == STM32BoardController::hdma_i2c2_tx.State)
	{
		static constexpr auto index = STM32BoardController::sem(STM32BoardController::Semafoor::Tx);
#ifndef FreeRTOSGebruikt
		STM32BoardController::i2c2Sem[index] = true;
#else
		CountingSemaphore &sem = STM32BoardController::i2c2Sem[index];
		STM32BoardController::isrSemafoorAfhandeling(sem);
#endif

	}
}

/**
* @brief This function handles DMA2 stream0 global interrupt.
*/
void DMA2_Stream0_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&STM32BoardController::hdma_spi1_rx);
}

/**
* @brief This function handles DMA2 stream1 global interrupt.
*/
void DMA2_Stream1_IRQHandler(void)
{
	// ?? HAL_UART_IRQHandler(&(STM32BoardController::huart6));
	HAL_DMA_IRQHandler(&STM32BoardController::hdma_usart6_rx);

	if(HAL_DMA_STATE_READY == STM32BoardController::hdma_usart6_rx.State)
	{
		static constexpr auto index = STM32BoardController::sem(STM32BoardController::Semafoor::Rx);
		STM32BoardController::huart6.gState = HAL_UART_STATE_READY; /* fix want hal bibliotheek doet dit niet ..... */
#ifndef FreeRTOSGebruikt
		STM32BoardController::usart6Sem[index] = true;
#else
		auto &sem = STM32BoardController::usart6Sem[index];
		STM32BoardController::isrSemafoorAfhandeling(sem);
#endif
	}
}

/**
* @brief This function handles DMA2 stream2 global interrupt.
*/
void DMA2_Stream2_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&STM32BoardController::hdma_spi1_tx);
}

/**
* @brief This function handles DMA2 stream7 global interrupt.
*/
void DMA2_Stream7_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&STM32BoardController::hdma_usart6_tx);

	if(HAL_DMA_STATE_READY == STM32BoardController::hdma_usart6_tx.State)
	{
		static constexpr auto index = STM32BoardController::sem(STM32BoardController::Semafoor::Tx);
		STM32BoardController::huart6.gState = HAL_UART_STATE_READY; /* fix want hal bibliotheek doet dit niet ..... */
#ifndef FreeRTOSGebruikt
		STM32BoardController::usart6Sem[index] = true;
#else
		auto &sem = STM32BoardController::usart6Sem[index];
		STM32BoardController::isrSemafoorAfhandeling(sem);
#endif
	}
}

#ifdef __cplusplus
}
#endif

ESEBoardController::ESEBoardController(const UInt32 freq) : STM32BoardController(freq),
                                                            ads131a02(*this),
                                                            max5136(*this),
                                                            ftdi(*this)
{

}


void ESEBoardController::stuurCommando()
{
	const DataPakket<UInt8> pakket(digProcesAansturing.maakCommandoPakket());
	verstuur(BoardI2C::BoardI2C1, pakket, DigProcesControllerI2CAdres);
}

void ESEBoardController::stuurProcesControle(const DigProcesAansturing::ProcesControle pc)
{
	digProcesAansturing.zetProcesControle(pc);

	stuurCommando();
}

void ESEBoardController::stuurProcesType(const DigProcesAansturing::ProcesType pt)
{
	digProcesAansturing.zetProcesType(pt);

	stuurCommando();
}

void ESEBoardController::stuurSampFreq(const UInt32 sf)
{
	digProcesAansturing.zetSampFreq(sf);

	stuurCommando();
}

void ESEBoardController::stuurNuldeOrdeParams(const float kpparam)
{
	digProcesAansturing.zetNuldeOrdeParams(kpparam);
	stuurCommando();
}

void ESEBoardController::stuurEersteOrdeParams(const float kpparam, const float tauparam)
{
	digProcesAansturing.zetEersteOrdeParams(kpparam,tauparam);
	stuurCommando();
}

void ESEBoardController::stuurTweedeOrdeParams(const float kpparam, const float omega0param, const float betaparam)
{
	digProcesAansturing.zetTweedeOrdeParams(kpparam,omega0param,betaparam);
	stuurCommando();
}

void ESEBoardController::muxTest()
{
#if (BoardVersie >= 11)
	selekteerInput(DSBSignaalSelektor::BNCSignaal);

	selekteerProces(RGTProcesSelektor::EersteOrdeProces);
	selekteerProces(RGTProcesSelektor::TweedeOrdeProces);
	selekteerProces(RGTProcesSelektor::DigProces);

	selekteerDACAntiAlias(AntiAliasCtrl::AA50kHz);
	selekteerDACAntiAlias(AntiAliasCtrl::AA5kHz);

	selekteerADCAntiAlias(AntiAliasCtrl::AA50kHz);
	selekteerADCAntiAlias(AntiAliasCtrl::AA5kHz);

	selekteerInput(DSBSignaalSelektor::LokaalDACSignaal);
	selekteerProces(RGTProcesSelektor::EersteOrdeProces);
#endif
}

void ESEBoardController::i2cTest()
{
	static const UInt8 testWaarden[] = { 1,2,3,4,5,6,7,8,9,10 };
	const DataPakket<UInt8> testPakket(const_cast<UInt8 *>(testWaarden),10);

	static const UInt8 test2Waarden[] = { 11,12,13,14,15,16,17,18,19,20 };
	const DataPakket<UInt8> test2Pakket(const_cast<UInt8 *>(test2Waarden),10);

	/**I2C1 GPIO Configuration
	PB6     ------> I2C1_SCL --> Bruin
	PB7     ------> I2C1_SDA --> Zwart */

	/**I2C2 GPIO Configuration
	PB10     ------> I2C1_SCL --> Bruin
	PB9      ------> I2C1_SDA --> Zwart */

	verstuur(BoardI2C::BoardI2C1, testPakket, DigProcesControllerI2CAdres);

	verstuur(BoardI2C::BoardI2C1, test2Pakket, DigProcesControllerI2CAdres);
}


#endif /* #ifndef BoardVersie */
