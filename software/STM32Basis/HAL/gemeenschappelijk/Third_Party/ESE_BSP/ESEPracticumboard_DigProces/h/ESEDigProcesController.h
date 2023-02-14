/******************************************************************************
 * Project        : HAN ESE Project 3 - Regeltechniek
 * File           : Basisklasse en startup routines van de STM32

 * Copyright      : 2010-2011 HAN Embedded Systems Engineering
 ******************************************************************************
  Change History:

    Version 2.0 - September 2011
    > STM32 versie van Fuzzy en Regeltechniek opdracht.

******************************************************************************/

#ifndef DigProcesController_H
#define DigProcesController_H

#include <algdef.h>
#include <dataPakket.h>
#include <stm32f3xx_hal_tim.h>
#include <stm32f3xx_hal_dma.h>
#include <stm32f3xx_hal_iwdg.h>
#include <stm32f3xx_hal_dac.h>
#include <stm32f3xx_hal_adc.h>


class STM32DigProcesController
{
public:

	typedef enum
	{
		Rood=0,
		Groen,
		LaatsteKleur
	} LEDKleur;

	typedef enum
	{
		KnipperLED_GeenKnipper=0,
		KnipperLED_langzaam,  /* 0,5 Hz */
		KnipperLED_middel,    /* 1 Hz */
		KnipperLED_snel,      /* 2 Hz */
		KnipperLED_zeerSnel,  /* 5 Hz */
		KnipperLED_Laatste
	} KnipperLEDStand;

	/*! @brief schakel een LED */
	void zetLED(const LEDKleur,
				const Schakelaar ,
				const KnipperLEDStand);


    /*! @brief wacht in stop mode/ slaap mode
     * @param stopMode = true --> wacht in STOP mode, false --> wacht in SLEEP mode */
    void wachtFunktie(const bool stopMode);

    /*! @brief Stel de Independent wachtdog in */
    void initWaakhond(const UInt16 prescaler = 8,
					  const UInt16 venster = 80);

	void aaiWaakhond();


	/*! @brief Ontvang unidirectioneel een UInt8 gebaseerd datapakket van de i2c1 (slaaf van de ESEBoardController)
	 * @param pakket dat naar de binnenkomende data wijst. */
	void ontvang(const DataPakket<UInt8> &);

	/*! @brief draai de byte volgorde van 4,3,2,1 om naar 1,2,3,4.
	 * @param Het om te draaien word.
	 * @return Het omgedraaide word.
	 */
	UInt32 revEndianess(const UInt32) const;

	/*! @brief start de ADC conversie.
	 *
	 * @param sampTijd . De sampling tijd in microseconden :
	 * @note de ondergrens is 10 ( ==> 100 kHz)
	 * @note de bovengrens is 1000000 ( ==> 1 Hz)
	 */
	void startADC(const UInt32 sampTijd);

	void wachtOpConversie();

	void stopADC();


	void startDAC();
	void stopDAC();

	/* alle DMA en IRQ handles */
	static bool i2c1RxSem;
	static bool i2c1TxSem;
	static DMA_HandleTypeDef hdma_i2c1_tx;
	static DMA_HandleTypeDef hdma_i2c1_rx;

	static TIM_HandleTypeDef htim17;

	static bool adcSem;
	static ADC_HandleTypeDef hadc1;
	static I2C_HandleTypeDef hi2c1;

protected:

	/*! @brief Constructor.
	 * @param De waarde van het gebruikte HSE crystal.
	 */
	STM32DigProcesController(const UInt32 cw=8000000);

	/*! @brief voor test : verstuur data met i2c */
	void verstuur(const DataPakket<UInt8> &ontvangPakket);


private:

	/*! @brief initialiseer de spi peripherals om de externe DAC en de ADC aan te spreken. */

	void gpioInit();
	void i2cInit();

    void adcInit();
    void dacInit();

	void tim1Init();
	void zettim1Frequentie(const KnipperLEDStand);

	void tim2Init();

    void slaapMode() const;

	/* herstel HSE/PLL na stop mode */
	void SYSCLKConfig_STOP();

    ErrorStatus HSEStartUpStatus;

	IWDG_HandleTypeDef waakhond;   /* de waakhond */


	DMA_HandleTypeDef hdma_adc1;

	DAC_HandleTypeDef hdac1;
	DMA_HandleTypeDef hdma_dac_ch1;

	TIM_HandleTypeDef htim1;
	TIM_HandleTypeDef htim2;
	TIM_HandleTypeDef htim3;

	TIM_OC_InitTypeDef ledOC[2]; /* voor de groene en de rode led */
	TIM_OC_InitTypeDef sampTimerOC;

	UInt16 adcWaarde;
	uint32_t uwIncrementState;


	const UInt32 crystalWaarde;

	static const UInt16 kleurKanaal[2];
	static const UInt16 EigenI2CAdres=0x77;
	static const UInt16 DSB_RGTControllerI2CAdres=0x6e; // (0xDC >>1)
	void zetDACwaarde(const UInt16);

	HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority);

	void HAL_ResumeTick(void);

	void HAL_SuspendTick(void);


};

class DigProcesController : public STM32DigProcesController
{
public:
	/*! @brief Constructor.
	 * @param De waarde van het gebruikte HSE crystal.
     * @param de PLL output (SYSCLK) frequentie in MHz. */
	DigProcesController(const UInt32 cw=8000000);

	void i2cTest();
	void adcTest();

private:

	UInt8 i2cOntvangstBuffer[512];
};



#endif /* stm32Controller_h */
