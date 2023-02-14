/*!
  @file
  Implementatie van de Board componenten drivers op het ESE RGT+DSB Board:
  Implementation of the Board component drivers on the ESE RGT + DSB Board:
  - TI ADS131A02 24 bit Sigma Delta A/D Converter
  - Maxim MAX 5136 16 bit D/A Converter

  @version $Rev: 4291 $
  @author $Author: ewout $

  @copyright Copyright 2009-2017 HAN Embedded Systems Engineering

  $Id: ESEBoardComponents.cpp 4291 2021-06-21 19:27:46Z ewout $
************************************************************************/

#include <ESEBoardComponents.h>
#include <ESEBoardController.h>


ADS131A02::ADS131A02(STM32BoardController &controllerRef) : controller(controllerRef),sampFreqGezet(false)
{

	/* start de reset en de ndrdy GPIO :
	 * nReset = PB0
	 * nDrdy = PC4 */
	GPIO_InitTypeDef GPIO_InitStruct;

	/* Peripheral clock enable */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* configureer de PC$ pin als EXTI om een interrupt te genereren bij nDRDY vanuit de ADC */
	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(EXTI4_IRQn, 9, 0);

	/* reset pin omhoog */
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
}

void ADS131A02::zetSampFreq(const ADS131A02::ICLK iclk,
							const ADS131A02::FMOD fmod,
							const ADS131A02::ODR odr)
{
	/* stuur NULL */
	stuurCommando(NULLCmd,0x02ff);
	stuurCommando(UnlockCmd,UnlockCmd);

	/* configureer het device */
	/* - gebruik GEEN negatieve charge pump.
	 * - high resolution mode.
	 * - Gebruik interne referentie Uref = 4V
	 * - over en onderspanning op 5% gezet
	 */
	static constexpr UInt8 Intern4VRef = (1<<4) +(1<<3);
	static constexpr UInt8 aSysCfg = (1<<5) + Intern4VRef;
	schrijfRegister(ASysCfgReg,aSysCfg);

	/* configureer de klok iclk  */
	const UInt8 clk1Cfg = (static_cast<UInt8>(iclk) << 1);
	schrijfRegister(CLK1Reg,clk1Cfg);
	/* Configureer de fmod en de ODR  */
	const UInt8 clk2Cfg = (static_cast<UInt8>(fmod) <<5)+static_cast<UInt8>(odr);   /* C++14 ! */
	schrijfRegister(CLK2Reg,clk2Cfg);

	sampFreqGezet = true;
}

void ADS131A02::start() const
{
	assert(true == sampFreqGezet);

    /* start de ADC */
	schrijfRegister(ADCENAReg,0b0011);

	/* stuur de wake-up */
	stuurCommando(WakeupCmd,WakeupCmd);

	/* zet de zaak op slot */
	stuurCommando(LockCmd,LockCmd);

#ifndef FreeRTOSGebruikt
	dataIsReady=false;
#else
	dataIsReady.Give();
#endif

	NVIC_EnableIRQ(EXTI4_IRQn);
}

void ADS131A02::start(const ADS131A02::ICLK iclk,
					  const ADS131A02::FMOD fmod,
					  const ADS131A02::ODR odr)
{

	/* zet de sampling freq */
	zetSampFreq(iclk,fmod,odr);

	/* start de ADC */
	start();
}

void ADS131A02::pauze()
{
	NVIC_DisableIRQ(EXTI4_IRQn);
	/* stuur NULL */
	//stuurCommando(NULLCmd,0x02ff);
	stuurCommando(UnlockCmd,UnlockCmd);

	/* stop de ADC conversie, maar houdt de setup intact */
	stuurCommando(StdByCmd,StdByCmd);

	//stuurCommando(LockCmd,LockCmd);
}

void ADS131A02::hervat()
{
	/* stuur NULL */
	stuurCommando(NULLCmd,0x02ff);
	//stuurCommando(UnlockCmd,UnlockCmd);

	/* Ga verder */
	stuurCommando(WakeupCmd,WakeupCmd);

	stuurCommando(LockCmd,LockCmd);

	NVIC_EnableIRQ(EXTI4_IRQn);
}


void ADS131A02::stop()
{
	NVIC_DisableIRQ(EXTI4_IRQn);

	/* stuur NULL */
	//stuurCommando(NULLCmd,0x02ff);
	stuurCommando(UnlockCmd,UnlockCmd);

	ONGEBRUIKT UInt8 clk1reg = leesRegister(CLK1Reg);
	ONGEBRUIKT UInt8 clk2reg = leesRegister(CLK2Reg);
	/* stop de ADC */
	schrijfRegister(ADCENAReg,0x0);
	stuurCommando(StdByCmd,StdByCmd);

	//stuurCommando(LockCmd,LockCmd);
}


void ADS131A02::zelfTest()
{
	/* spi1 (ADC) test. */
	/* MOSI = Geel = PA7 .
	 * MISO = Groen = PA6 .
	 * SCLK = Blauw = PA5 .
	 * nCS = Paars = PA4 .
	 * CPOL = 0  CPHA = 1 */
	/* stuur een reset */

	//ads.stop();
	/* wens is 20 kHz ==> 16,384 MHz ..2..  8,192 ..2.. 4,096 ..200.. 20,48 kHz */
	//ads.start(ADS131A02::ICLK2, ADS131A02::FMOD2, ADS131A02::ODR200);

	/* wens is 20 Hz ==> 16,384 MHz ..14..  1,17 ..14.. 83 kHz ..4096.. 20,4 Hz */
	start(ICLK::ICLK14, FMOD::FMOD14, ODR::ODR4096);

	/* vraag de status op */
//	retwaarde = controller.verstuur(STM32BoardController::BoardSPI2,0x2987);

	/* -3- Toggle IO in an infinite loop */
	for (auto i=0;i<50;i++)
	{
		wachtOpDataReady();
		HAL_GPIO_TogglePin(LEDx_GPIO_PORT, LED1_PIN);
		laadConversieData();
		ONGEBRUIKT const WFT spanning1 = konverteer(Kanaal::K1);
		ONGEBRUIKT const WFT spanning2 = konverteer(Kanaal::K2);
	}

	pauze();

	hervat();

	stop();
	
}

void ADS131A02::reset()
{
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
	HAL_Delay(500);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
}

void ADS131A02::stuurCommando(const ADS131A02::Commando cmd,
							  const UInt16 ack) const
{
	UInt16 response;
	do
	{
		controller.verstuur(STM32BoardController::BoardSPI::BoardSPI1, cmd);
		const UInt32 unlock = controller.verstuur(STM32BoardController::BoardSPI::BoardSPI1,NULLCmd);

		if (NULLCmd == cmd)
			response = ack;   /* negeer de verwachte uitslag want null levert status op en die s niet altijd bekend */
		else
		{
			if (ack != (response = static_cast<UInt16>(unlock)))
				HAL_Delay(10);
		}
	} while(ack != response);
}

UInt8 ADS131A02::leesRegister(const ADS131A02::RegisterAdres reg) const
{
	assert(reg < EindeRegisters);

	const UInt32 cmd = ((RRegBasisCmd+reg));

	controller.verstuur(STM32BoardController::BoardSPI::BoardSPI1, cmd);
	const UInt32 waarde = controller.verstuur(STM32BoardController::BoardSPI::BoardSPI1,NULLCmd);

	const UInt8 retwaarde = static_cast<UInt8>(0xff & (waarde>>8));

	return(retwaarde);
}

void ADS131A02::schrijfRegister(const ADS131A02::RegisterAdres reg, const UInt8 waarde) const
{
	assert(reg < EindeRegisters);

	const UInt32 cmd = ((WRegBasisCmd+reg)+ (waarde<<8));

	controller.verstuur(STM32BoardController::BoardSPI::BoardSPI1, cmd);
	const UInt32 retwaarde = controller.verstuur(STM32BoardController::BoardSPI::BoardSPI1,NULLCmd);
	ONGEBRUIKT const UInt8 geschrevenwaarde = static_cast<UInt8>(0xff & (retwaarde>>8));
	assert(waarde == geschrevenwaarde);
}

#ifndef FreeRTOSGebruikt
volatile bool ADS131A02::dataIsReady;
#else
cpp_freertos::CountingSemaphore ADS131A02::dataIsReady;
#endif

void ADS131A02::dataReady()
{
#ifndef FreeRTOSGebruikt
	dataIsReady=true;
#else
	BaseType_t wakkerGeworden;
	dataIsReady.GiveFromISR(&wakkerGeworden);
	if (true == wakkerGeworden)
		portYIELD_FROM_ISR( wakkerGeworden );
#endif
}

void ADS131A02::wachtOpDataReady() const
{
#ifndef FreeRTOSGebruikt
	while (false == dataIsReady);
	dataIsReady=false;
#else
	dataIsReady.Take();
#endif
}

void ADS131A02::laadConversieData()
{
	static const UInt32 uitData[3] = { NULLCmd, 0x0,0x0};
	const DataPakket<UInt32> uitPakket(const_cast<UInt32 *>(uitData),3);

	ADCWaarde inData[3];
	const DataPakket<ADCWaarde> inPakket(const_cast<ADCWaarde *>(inData),3);

	controller.verstuur(uitPakket,inPakket);

	adcData[0] = (controller.revEndianess(inPakket[1]) >> 8);
	adcData[1] = (controller.revEndianess(inPakket[2]) >> 8);
}

void ADS131A02::laadConversieData(ADCData &adcDataKopie)
{
	laadConversieData();
	adcDataKopie = adcData;
}


ADS131A02::ADCWaarde ADS131A02::operator[](const ADS131A02::Kanaal kanaal) const
{
	const auto index = static_cast<Teller>(kanaal);
	const UInt32 uit = adcData[index];
	return(uit);
}

ADS131A02::Spanning ADS131A02::konverteer(const ADS131A02::Kanaal kanaal) const
{
	/* extern aangeboden : 0 .. 5V.
	 * door SA blok verkleind tot 0 .. 4V.
	 * resultaat = 0x0 ==> 0V  SA ==> 0V signaal
	 * resultaat = 0x7fffff ==> 4V SA ==> 5V signaal */
	const auto index = static_cast<Teller>(kanaal);
	const ADCWaarde adcWaarde = adcData[index];
	const Spanning spanning = (adcWaarde*5.0f)/0x7fffff;   /* maak er een floating point operatie van */
	return(spanning);
}





/* nDRDY interrupt op PC4 */
extern "C" void EXTI4_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
	ADS131A02::dataReady();
}


MAX5136::MAX5136(STM32BoardController &controllerRef) : controller(controllerRef)
{
	/* MAX516 op SPI2 */
	/* Extra pinnen :
	 * PB14 : nReady ==> DAC klaar met inladen van de data.
	 * PC0 : nLDAC ==> Laad de dac waarde uit het register in de D/A operator */
	GPIO_InitTypeDef GPIO_InitStruct;

	/* Peripheral clock enable */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	/* configureer de PC$ pin als EXTI om een interrupt te genereren bij nDRDY vanuit de ADC */
	GPIO_InitStruct.Pin = GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 9, 0);

	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/* nLDAC pin omhoog */
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_SET);
}

void MAX5136::reset()
{
	static const UInt8 clr[3] = { CLR, 0x0,0x0};
	static const UInt8 slaap[3] = { PwrCtrl, 0b11,(1<<7)};

	const DataPakket<UInt8> clrData(const_cast<UInt8 *>(clr),3);
	const DataPakket<UInt8> slaapData(const_cast<UInt8 *>(slaap),3);

	controller.verstuur(STM32BoardController::BoardSPI::BoardSPI2, clrData);
	controller.verstuur(STM32BoardController::BoardSPI::BoardSPI2, slaapData);
}

void MAX5136::start(const MAX5136::Kanaal kanaal)
{
	static constexpr UInt8 masker = 0x3;
	const UInt8 wakkerGeworden = (masker & ~static_cast<UInt8>(kanaal));
	const UInt8 wakker[3] = { PwrCtrl, wakkerGeworden,(1<<7)};

	const DataPakket<UInt8> wakkerData(const_cast<UInt8 *>(wakker),3);

	controller.verstuur(STM32BoardController::BoardSPI::BoardSPI2, wakkerData);
}

void MAX5136::stop(const MAX5136::Kanaal kanaal)
{
	static constexpr UInt8 masker = 0x3;
	const UInt8 inSlaapGevallen = (masker & static_cast<UInt8>(kanaal));
	const UInt8 slaap[3] = { PwrCtrl, inSlaapGevallen,(1<<7)};

	const DataPakket<UInt8> slaapData(const_cast<UInt8 *>(slaap),3);

	controller.verstuur(STM32BoardController::BoardSPI::BoardSPI2, slaapData);
}

void MAX5136::laadKanaal(const MAX5136::Kanaal kanaal, const DACWaarde waarde)
{
	DACData dacData;
	dacData[0] = Write+static_cast<UInt8>(kanaal);
	dacData[1] = static_cast<UInt8>(waarde>>8);
	dacData[2] = static_cast<UInt8>(0xff & waarde);

	controller.verstuur(STM32BoardController::BoardSPI::BoardSPI2,dacData);
}

void MAX5136::zetKanaal(const MAX5136::Kanaal kanaal, const DACWaarde waarde)
{
	DACData dacData;
	dacData[0] = WriteThrough+static_cast<UInt8>(kanaal);
	dacData[1] = static_cast<UInt8>(waarde>>8);
	dacData[2] = static_cast<UInt8>(0xff & waarde);

	controller.verstuur(STM32BoardController::BoardSPI::BoardSPI2,dacData);
}


void MAX5136::zetKanaal(const MAX5136::Kanaal kanaal, const DACWaarde waarde,const DACWaarde offset)
{
	const DACWaarde totaal = waarde+offset;
	zetKanaal(kanaal,totaal);
}

void MAX5136::zetSpanning(const Kanaal kanaal, const WFT spanning, const bool autoBegrens)
{
	WFT bgSpanning = spanning;
	if (true==autoBegrens)
	{
		if (bgSpanning<0.0f)
			bgSpanning=0.0f;
		else if (bgSpanning>5.0f)
			bgSpanning=5.0f;
	}

	const DACWaarde waarde = convert(bgSpanning);
	zetKanaal(kanaal,waarde);
}

void MAX5136::zetKanaal(const DACWaarde kanaal1, const DACWaarde kanaal2)
{
	laadKanaal(Kanaal::K1,kanaal1);
	laadKanaal(Kanaal::K2,kanaal2);

	static const UInt8 ldac[3] = { LDAC, 0b11,0x0};

	/* stuur nLDAC commando om de waarden op de analoge uitgang te zetten */
	const DataPakket<UInt8> ldacData(const_cast<UInt8 *>(ldac),3);
	controller.verstuur(STM32BoardController::BoardSPI::BoardSPI2,ldacData);

	/* HW strobe - vervangen door nLDAC SW commando hierboven */
	//HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_RESET);
	//HAL_Delay(1);
	//HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_SET);
}

void MAX5136::zetSpanning(const WFT spanning1, const WFT spanning2)
{
	const DACWaarde waarde1 = convert(spanning1);
	const DACWaarde waarde2 = convert(spanning2);

	zetKanaal(waarde1,waarde2);
}

void MAX5136::zelfTest()
{
/* spi2 (DAC) test. */
	/* MOSI = Geel = PC3 .
	 * SCLK = Blauw = PB13 .
	 * nCS = Paars = PB12
	 * nReady = PB14
	 * nLDAC = PB0. */
	/* stuur een reset */

	reset();

	start(Kanaal::AlleKanalen);

	zetKanaal(Kanaal::K1,0xffff);

	zetKanaal(Kanaal::K2,0x3456);

	zetKanaal(0x6789,0x5678);

	zetSpanning(Kanaal::K1, 2.0f, 0);
	zetSpanning(Kanaal::K2, 3.75f, 0);
}

WFT MAX5136::dacSpanning(const MAX5136::DACWaarde dacWaarde)
{
	const auto waarde = static_cast<WFT>((dacWaarde * 5.0f) / 0xffff);

	return(waarde);
}

MAX5136::DACWaarde MAX5136::convert(const WFT spanning)
{
	assert((spanning >= 0.0f) && (spanning<= 5.0f));

	const auto waarde = static_cast<DACWaarde>((spanning/5.0f) * 0xffff);

	return(waarde);
}


FTDI232BQ::FTDI232BQ(STM32BoardController &controllerRef) : controller(controllerRef)
{

}

void FTDI232BQ::reset()
{

}


void FTDI232BQ::verstuurByte(const UInt8 data)
{
	controller.verstuurByte(STM32BoardController::BoardUART::BoardUART6,data);
}

void FTDI232BQ::verstuur(const FTDI232BQ::FTDIData &data)
{
	controller.verstuur(STM32BoardController::BoardUART::BoardUART6,data);
}

UInt8 FTDI232BQ::ontvangByte()
{
	const UInt8 data = controller.ontvangByte(STM32BoardController::BoardUART::BoardUART6);
	return(data);
}

void FTDI232BQ::ontvang(const FTDI232BQ::FTDIData &data)
{
	controller.ontvang(STM32BoardController::BoardUART::BoardUART6,data);
}

void FTDI232BQ::zelfTest()
{
	/**USART6 GPIO Configuration
	PC6     ------> USART6_TX
	PG9     ------> USART6_RX
	PG12     ------> USART6_RTS
	PG13     ------> USART6_CTS
 **/
	static char const * const testTekst = "HAN Embedded Systems Engineering is een geweldige HBO opleiding.";

	const DataPakket<UInt8> testPakket(const_cast<UInt8 * const>(reinterpret_cast<UInt8 const * const>(testTekst)),
									  strlen(testTekst));

	verstuur(testPakket);
}



