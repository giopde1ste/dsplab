/*!
@file
API van de c++ schil voor gebruik van de ESE Board kit.
@version $Rev: 3674 $
@author $Author: ewout $

@copyright Copyright 2017-2018 Hogeschool van Arnhem en Nijmegen, ir drs E.J Boks

$URL: http://svn.hanese.nl/docenten/svn/ewout/vakken/algemeen/software/STM32/STM32Basis/HAL/gemeenschappelijk/Third_Party/ESE_BSP/ESEPracticumboard_RGT+DSB/h/ESEBoardController.h $
$Id: ESEBoardController.h 3674 2019-10-29 14:35:28Z ewout $
************************************************************************/

#ifndef ESEBoardController_H
#define ESEBoardController_H


#include <algdef.h>
#include <dataPakket.h>

#include <DigProcesAansturing.h>
#include <STM32412gDiscovery.h>

#include "ESEBoardComponents.h"
#ifdef SeggerRTTAanwezig
#include <RTT/SEGGER_RTT.h>
#endif

#ifdef FreeRTOSGebruikt
using namespace cpp_freertos;
#endif

#ifndef StemWinAanwezig
#include <stm32412g_discovery.h>
using BoardControllerBasis = STM32412gDiscovery;
#else
#include <h/STM32412gDiscoveryGUI.h>
using BoardControllerBasis = STM32412DiscoveryMetGUI;
#endif

/*! @class Deze klasse vertegenwoordigt een low-level driver voor het ESE RGT+DSB practicum board.
 * Studenten hoeven (waarschijnlijk) niet direkt de methoden van deze klasse te gebruiken,
 * maar zullen de afgeleide klassen gebruiken.
 * English : This class represents a driver for the ESE RGT + DSB practicum board.
 * Students (probably) do not need to use the methods of this class directly,
 * but will use the derived classes. */
class STM32BoardController : public BoardControllerBasis
{
public:

	/*! @enum SPI1 en SPI2 selektor */
	enum class BoardSPI
	{
		BoardSPI1=0,
		BoardSPI2,
		LaatsteSPI
	};

	/*! @enum UART2 (DBG) en UART6 (FTDI ==> USB) selektor */
	enum class BoardUART
	{
		BoardUART2=0,  /* Debug UART */
		BoardUART6,    /* FTDI  */
		LaatsteUART
	};

	/*! @enum I2C1 (DigProces STM32F0 Controller) en I2C2 (Reserve) selektor */
	enum class BoardI2C
	{
		BoardI2C1=0,
		BoardI2C2,
		LaatsteI2C
	};

	/*! @brief Verstuur een UInt32 naar de TI ADS131A02 (ADC) of de MAX5136 (DAC).
	 * @return terugggestuurde waarde (allen bij ADC van toepassing) */
	UInt32 verstuur(const BoardSPI,
	                const UInt32 );

	/*! @brief Verstuur bidirectioneel een datapakket naar de TI ADS131A02 .
	 * @param pakket dat naar de uitgaande data ( uC --> ADC of DAC) wijst.
	 * @param pakket dat naar de inkomende data ( ADC --> uC) wijst */
	void verstuur(const DataPakket<UInt32> &,
	              const DataPakket<UInt32> &);

	/*! @brief Verstuur unidirectioneel een 8bit gebaseerd datapakket naar de TI ADS131A02 (ADC) of de MAX5136 (DAC).
     * @param de SPI slaaf in kwestie.
	 * @param pakket dat naar de uitgaande data ( --> ADC of DAC) wijst. */
	void verstuur(const BoardSPI eh,
	              const DataPakket<UInt8> &);

	/*! @brief Verstuur unidirectioneel een 32bit gebaseerd datapakket naar de TI ADS131A02 (ADC) of de MAX5136 (DAC).
	 * @param de SPI slaaf in kwestie.
	 * @param pakket dat naar de uitgaande data ( --> ADC of DAC) wijst. */
	void verstuur(const BoardSPI,
	              const DataPakket<UInt32> &);

	/*! @brief Verstuur unidirectioneel een UInt8 gebaseerd datapakket naar de uart2 (DBG connector via J-Link)
	 * of USART6 (FTDI omzetting naar USB naar PC) .
 	 * @param de SPI slaaf in kwestie.
 	 * @param pakket dat naar de uitgaande data wijst. */
	void verstuur(const BoardUART eh,
	              const DataPakket<UInt8> &);

	/*! @brief Verstuur unidirectioneel een UInt8 naar de uart2 (DBG connector via J-Link)
     * of USART6 (FTDI omzetting naar USB naar PC) .
     * @param de SPI slaaf in kwestie.
     * @param Het uitgaande byte. */
	void verstuurByte(const BoardUART eh,
	                  const UInt8 );

	/*! @brief Ontvang unidirectioneel een char gebaseerd datapakket van de uart2 (DBG connector via J-Link)
	 * of USART6 (FTDI omzetting naar USB) .
 	 * @param de UART in kwestie. */
	UInt8 ontvangByte(const BoardUART eh);

	/*! @brief Ontvang unidirectioneel een char gebaseerd datapakket van de uart2 (DBG connector via J-Link)
	 * of USART6 (FTDI omzetting naar USB) .
 	 * @param de UART in kwestie.
 	 * @param pakket dat naar de binnenkomende data wijst. */
	void ontvang(const BoardUART eh,
	             const DataPakket<UInt8> &);

	/*! @brief Verstuur unidirectioneel een 8bit gebaseerd datapakket naar de DigProces Controller (of de reserve) .
 	 * @param de I2C slaaf in kwestie.
 	 * @param pakket dat naar de uitgaande data ( --> DigProces Controller) wijst.
 	 * @param Het 7 bit slaaf adres. */
	void verstuur(const BoardI2C eh,
	              const DataPakket<UInt8> &,
	              const UInt8 slaafAdres);

	/*! @brief Ontvang unidirectioneel een 8bit gebaseerd datapakket van de DigProces Controller (of de reserve) .
 	 * @param de I2C slaaf in kwestie.
	 * @param pakket dat naar de inkomende data (DigProces Controller -->) wijst.
	 * @param Het 7 bit slaaf adres.*/
	void ontvang(const BoardI2C eh,
	             const DataPakket<UInt8> &,
	             const UInt8 slaafAdres);

	enum class DSBSignaalSelektor
	{
		BNCSignaal=0,
		LokaalDACSignaal,
		AuxSignaal
	};

	/*! @brief selekteer een signaal voor gebruik in de DSB toepassing */
	void selekteerInput(const DSBSignaalSelektor) const;

	enum class RGTProcesSelektor
	{
		EersteOrdeProces=0,
		TweedeOrdeProces,
		DigProces
	};

	void selekteerProces(const RGTProcesSelektor ) const;

	enum class AntiAliasCtrl
	{
		AA50kHz=0,
		AA5kHz
	};

	/*! @brief Selekteer de Anti Alias filterconfiguratie voor de ADC op het board.
	 * @param De gewenste Anti Alias configuratie.
	 */
	void selekteerADCAntiAlias(const AntiAliasCtrl) const;

	/*! @brief Selekteer de Anti Alias filterconfiguratie voor de DAC op het board.
	 * @param De gewenste Anti Alias configuratie.
	 */
	void selekteerDACAntiAlias(const AntiAliasCtrl);

	DigProcesAansturing digProcesAansturing;

	static SPI_HandleTypeDef hspi1;
	static SPI_HandleTypeDef hspi2;

	static UART_HandleTypeDef huart2;
	static DMA_HandleTypeDef hdma_usart2_tx;
	static DMA_HandleTypeDef hdma_usart2_rx;

	static UART_HandleTypeDef huart6;
	static DMA_HandleTypeDef hdma_usart6_tx;
	static DMA_HandleTypeDef hdma_usart6_rx;

	static DMA_HandleTypeDef hdma_spi1_tx;

	static DMA_HandleTypeDef hdma_spi1_rx;
	static DMA_HandleTypeDef hdma_spi2_tx;

	static I2C_HandleTypeDef hi2c1;
	static I2C_HandleTypeDef hi2c2;
	static DMA_HandleTypeDef hdma_i2c1_tx;
	static DMA_HandleTypeDef hdma_i2c1_rx;
	static DMA_HandleTypeDef hdma_i2c2_tx;
	static DMA_HandleTypeDef hdma_i2c2_rx;

	enum class Semafoor
	{
		Rx=0,
		Tx,
		LaatsteSem
	};
	static constexpr auto AantalSems = static_cast<Teller>(Semafoor::LaatsteSem);

	static constexpr inline Teller sem(const Semafoor s)
	{
		return(static_cast<Teller>(s));
	};

#ifndef FreeRTOSGebruikt
	static volatile bool spi1Sem;
	static volatile bool spi2Sem;

	static volatile bool usart2Sem[AantalSems];
    static volatile bool usart6Sem[AantalSems];

	static volatile bool i2c1Sem[AantalSems];
	static volatile bool i2c2Sem[AantalSems];
#else


	static CountingSemaphore usart2Sem[AantalSems];
	static CountingSemaphore usart6Sem[AantalSems];

	static CountingSemaphore i2c1Sem[AantalSems],i2c2Sem[AantalSems];
	static CountingSemaphore spi1Sem,spi2Sem;
	static CountingSemaphore knopSems[static_cast<Teller>(KnopID::AantalKnoppen)];
#endif

protected:
	
	explicit STM32BoardController(const UInt32 freq=96);

	static constexpr UInt16 DigProcesControllerI2CAdres=0x77;

private:

	/*! @brief initialiseer de gpio voor I/O akties zoals anti alias ctrl en rgt ctrl */
	void gpioInit();

	/*! @brief initialiseer de spi peripherals om de externe DAC en de ADC aan te spreken. */
	void spiInit();

	void i2cInit();

	void uart2Init();

	/*! @brief init de uart die via ftdi met de desktop verbindt. */
	void usart6Init();

	void spiSemWacht(const STM32BoardController::BoardSPI);

#ifndef FreeRTOSGebruikt
	static KnopID knop;	  /* bevat de stand van de drukknop als ware het een knop met geheugen */
#endif
	static volatile bool timer2Afgelopen;

	static const UInt16 EigenI2CAdres=0x6e;
	
	static constexpr auto RTTBufferGrootte = 128;
	UInt8 RTTtekstBuffer[RTTBufferGrootte];
	UInt8 RTTloggingBuffer[RTTBufferGrootte];
	
	static constexpr UInt32 TerminalBufferIndex=0;
	static constexpr UInt32 LoggingBufferIndex=1;
};

/*! @class Deze klasse vertegenwoordig een driver voor gebruik van het ESE RGT + DSB practicum board
 * bij het Embedded Systems Engineering onderwijs.
 * @note Studenten hoeven (waarschijnlijk) niet direkt de methoden van deze klasse te gebruiken,
 * maar zullen de afgeleide klassen gebruiken.
 * Let wel op : De ADC ADS131A02 en DAC MAX5136 worden wel direkt gebruikt bij de vakken RGT/ECS en DSB.
 * English : This class represents a driver for the ESE RGT + DSB practicum board in
 * Embedded Systems Engineering education.
 * Students (probably) do not need to use the methods of this class directly,
 * but will use the derived classes.
 * Please note: The ADC ADS131A02 and DAC MAX5136 are used directly for the RGT / ECS and DSB courses.*/
class ESEBoardController : public STM32BoardController
{
public:
	/*! @brief Constructor voor deze klasse.
     * @param de PLL output (SYSCLK) frequentie in MHz. */
	explicit ESEBoardController(const UInt32 klok=96);

	/*! @brief Onderstaande funkties zijn bedoeld voor gebruik tijdens het regeltechniek practicum.
	 * The functions below are intended for use during the embedded control systems practical work.*/
	
	void stuurProcesControle(const DigProcesAansturing::ProcesControle );
	void stuurProcesType(const DigProcesAansturing::ProcesType );
	void stuurSampFreq(const UInt32);

	/*! @brief zet de Kp van het nuldeordeproces.
 	 * @param Kp*/
	void stuurNuldeOrdeParams(const float );

	/*! @brief zet de Kp en de tau van het eersteordeproces.
	 * @param Kp.
	 * @param tau. */
	void stuurEersteOrdeParams(const float,const float);

	/*! @brief zet de Kp, omega0 en beta van het tweedeordeproces.
	 * @param Kp.
	 * @param omega0.
	 * @param beta */
	void stuurTweedeOrdeParams(const float,const float,const float);

	/* stuur een teststring naar de DigProces Controller */
	void i2cTest();
	void muxTest();

protected:

	/* board componenten */
	ADS131A02 ads131a02;
	MAX5136 max5136;
	FTDI232BQ ftdi;


	void stuurCommando();

};

#endif /* stm32Controller_h */
