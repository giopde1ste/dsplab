/*!
@file
API van de c++ schil voor gebruik van de STM32412g Discovery kit.
@version $Rev: 3639 $
@author $Author: ewout $

@copyright Copyright 2017-2018 Hogeschool van Arnhem en Nijmegen, ir drs E.J Boks

$URL: http://svn.hanese.nl/docenten/svn/ewout/vakken/algemeen/software/STM32/STM32Basis/HAL/gemeenschappelijk/Libraries/BSP/STM32412G-Discovery/h/STM32412gDiscovery.h $
$Id: STM32412gDiscovery.h 3639 2019-09-27 15:44:58Z ewout $
************************************************************************/

#ifndef STM32412gDiscovery_H
#define STM32412gDiscovery_H

#include <stm32412g_discovery.h>
#include <algdef.h>
#include <stm32f4xx_hal_qspi.h>
#include <stm32f4xx_hal_rng.h>
#include <stm32f4xx_hal_sd.h>
#include <dataPakket.h>
#include <stm32f4xx_hal_rtc.h>
#include <stm32f4xx_hal_iwdg.h>
#include <CortexMProcessor.h>


#ifdef FreeRTOSGebruikt
#include <semaphore.hpp>
using namespace cpp_freertos;
#endif


class STM32412gDiscovery : public CortexMProcessor
{
public:

	
	enum class LEDKleur
	{
		Green=0,
		Orange,
		Red,
		Blue,
		AllColours,
		LaatsteKleur
	} ;

	using LEDColour = LEDKleur ;

	enum class KnipperLEDStand
	{
		Stop=0,
		PermanentAan,
		Langzaam,
		Middel,
		Snel,
		ZeerSnel,
		Laatste
	};

	/*! @brief Schakel LED 4 (Blauwe LED) naar een knipperstand.
	  @param de knipperstand */
	void knipperBlauwLED(const KnipperLEDStand) const;

	/*! @brief schakel een LED */
	void setLED(const LEDKleur,
	            const Schakelaar stand) const;

	/*! @brief  Deze funkties worden aangeroepen in de EXTI0 IRQ/TIM2IRQ handler */
	enum class KnopID
	{
		Centrum=0,
		Onder,
		Links,
		Rechts,
		Boven,
		AantalKnoppen,
		Onbekend
	};

	/*! @brief wacht op een willekeurige knop in stop mode/ slaap mode op knopdruk.
     * @param stopMode = true --> wacht in STOP mode, false --> wacht in SLEEP mode */
	KnopID wachtOpEenKnop(const bool stopMode);

	/*! @brief wacht op een bepaalde knop in stop mode/ slaap mode op knopdruk.
     * @param stopMode = true --> wacht in STOP mode, false --> wacht in SLEEP mode */
	void wachtOpKnop(const KnopID, const bool stopMode);

	Schakelaar geefKnopStand(const KnopID) const;


	/*! @brief wacht in stop mode/ slaap mode
	 * @param stopMode = true --> wacht in STOP mode, false --> wacht in SLEEP mode */
	void wachtFunktie(const bool stopMode);

	/*! @brief wacht funktie gebaseerd op timer 4
	 *  @param  Wachttijd in millisecs.
	 *  @param Wacht in slaapmode (true = sleepmode, false = polling) */
	void wachtFunktie(const unsigned short,
	                  const bool slaap=true) const;

	/*! @brief Stel de Independent wachtdog in */
	void initWaakhond(const UInt16 prescaler = 8,
	                  const UInt16 venster = 80);

	/*! @brief zorg er voor dat de lopende watchdog niet een HW reset veroorzaakt. */
	void aaiWaakhond();

	/*! @brief voer een HW reset uit op deze processor.
	 * @warning Er is gee nweg terug. Het programma stopt en start opnieuw !
	 */
	void coreReset() const;

	/*! @brief Sla gegevens op het in BKUP geheugen.
	 * @param Het backup adres
	 * @param De waarde
	 * @return Status van de schrijf operatie.
	 */
	FoutCode backupGeheugenOpslaan(const UInt16, const UInt32 );

	/*! @brief Lees gegevens uit het in BKUP geheugen.
	 * @param Het backup adres.
	 * @return de waarde. */
	UInt32 backupGeheugenLezen(const UInt16 );

	/*! @brief print een tekst via het ITM debug kanaal */
	static UInt32 ITMPrint(const UInt32 ch);
	static void ITMPrint(const char *);


#ifndef FreeRTOSGebruikt
	static KnopID knopGedrukt(const KnopID);
#endif
	static void timer2IsAfgelopen();

	/*! @brief draai de byte volgorde van 4,3,2,1 om naar 1,2,3,4.
	 * @param Het om te draaien word.
	 * @return Het omgedraaide word.
	 */
	UInt32 revEndianess(const UInt32) const;


	static TIM_HandleTypeDef Tim12Handle; /* voor wachtfunktie */
	static TIM_HandleTypeDef Tim13Handle; /* voor knipper van LED 3 */
	static TIM_HandleTypeDef Tim14Handle; /* voor HAL Tick */

	static DMA_HandleTypeDef hdma_sdio_tx;
	static DMA_HandleTypeDef hdma_sdio_rx;

#ifndef FreeRTOSGebruikt

#else
	static CountingSemaphore knopSems[static_cast<Teller>(KnopID::AantalKnoppen)];
	static void isrSemafoorAfhandeling(CountingSemaphore &sem);
#endif
protected:

	/*! @brief Constructor.
     * @param de PLL output (SYSCLK) frequentie in MHz. */
	explicit STM32412gDiscovery(const UInt32 );

	KnopID geefKnopStand() const;
	void resetKnop();

	void enableSWO(const UInt32 targetDiv,const UInt32 itmPort=0);

	RTC_HandleTypeDef hrtc;

	static const UInt16 DigProcesControllerI2CAdres=0x77;
	
	void eepromInit();
	
private:

	/*! @brief initialiseer de gpio voor I/O akties zoals antia alias ctrl en rgt ctrl */
	void gpioInit();

	void ledsInit();

	void qspiInit();

	void rngInit();

	void sdioInit();

	void rtcInit();

	
	
	void slaapMode() const;

	/**
     * @brief  This function configures the TIM14 as a time base source.
     *         The time source is configured  to have 1ms time base with a dedicated
     *         Tick interrupt priority.
     * @note   This function is called  automatically at the beginning of program after
     *         reset by HAL_Init() or at any time when clock is configured, by HAL_RCC_ClockConfig().
     * @param  TickPriority: Tick interrupt priorty.
     * @retval HAL status
     */
	HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority);


	/* herstel HSE/PLL na stop mode */
	void SYSCLKConfig_STOP(const UInt32 freq);

	ErrorStatus HSEStartUpStatus;

	IWDG_HandleTypeDef waakhond;   /* de waakhond */

	QSPI_HandleTypeDef hqspi;

	RNG_HandleTypeDef hrng;

	SD_HandleTypeDef hsd;
	HAL_SD_CardInfoTypeDef SDCardInfo;

#ifndef FreeRTOSGebruikt
	static KnopID knop;	  /* bevat de stand van de drukknop als ware het een knop met geheugen */
#endif
	static bool timer2Afgelopen;

	static constexpr UInt16 EigenI2CAdres=0x6e;

};

#endif /* stm32Controller_h */
