/*!
@file
API van de c++ schil voor gebruik van de STM32412g Discovery MET GUI kit.
@version $Rev: 3590 $
@author $Author: ewout $

@copyright Copyright 2017-2019 Hogeschool van Arnhem en Nijmegen, ir drs E.J Boks

$URL: http://svn.hanese.nl/docenten/svn/ewout/vakken/algemeen/software/STM32/STM32Basis/HAL/gemeenschappelijk/Libraries/BSP/STM32412G-Discovery/h/STM32412gDiscovery.h $
$Id: STM32412gDiscovery.h 3590 2019-09-05 15:47:45Z ewout $
************************************************************************/

#ifndef STM32412gDiscoveryGUI_H
#define STM32412gDiscoveryGUI_H

#ifndef FreeRTOSGebruikt
#error "(STM32412gDiscoveryGUIDriver) Voor het gebruik van deze module is FreeRTOS vereist."
#else
#include "STM32412gDiscovery.h"
#include "stm32412g_discovery_ts.h"
#endif

class SchermKalibratie : public TS_StateTypeDef
{
public:
	explicit SchermKalibratie(STM32412gDiscovery &);

	bool kalibreer();

	uint16_t kalibreerX(const uint16_t x) const;
	uint16_t kalibreerY(const uint16_t y) const;

	bool isGekalibreerd() const;

private:
	STM32412gDiscovery &discovery;
	
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
	bool ts_calibration_done=false;

	typedef struct
	{
		UInt16 x;
		UInt16 y;
	} MeetPunt;

	MeetPunt meetpunten[2];

/* Private function prototypes -----------------------------------------------*/
	void setHint();

	void leesTouchWaarden(const Teller,
	                      UInt16 &,
	                      UInt16 &);

	void wachtOpDrukStaat(uint8_t Pressed) ;
/* Private functions ---------------------------------------------------------*/

	static constexpr Int16 kalibPunten[2] = {20,220};
	static constexpr Int16 pte = kalibPunten[1]-kalibPunten[0];
};

class STM32412gDiscoveryGUIDriver : public SchermKalibratie
{
public:
	explicit STM32412gDiscoveryGUIDriver(STM32412gDiscovery &);

	/*! @brief voer de init uit nadat het rtos is opgestart. */
	void postOSStartGUIInit();

	/* Blijf wachten tot er een geldige scherm touch is geweest. */
	TS_StateTypeDef wachtOpSchermTouch() const;

	static CountingSemaphore schermTouch;


};

/*! @brief DE Discovery, maar met toevoeging van de GUI komponenten. */
class STM32412DiscoveryMetGUI : public STM32412gDiscovery
{
public:
	/*! @brief initialiseer de klasse.
	 * @param De PLL frequentie in MHz */
	explicit STM32412DiscoveryMetGUI(const UInt32 );

	STM32412gDiscoveryGUIDriver guiDriver;

private:

};

#endif /* stm32Controller_h */
