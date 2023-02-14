/*!
@file
Implementatie van de c++ schil voor gebruik van de STM32412g Discovery kit.
@version $Rev: 3592 $
@author $Author: ewout $

@copyright Copyright 2017-2018 Hogeschool van Arnhem en Nijmegen, ir drs E.J Boks

$URL: http://svn.hanese.nl/docenten/svn/ewout/vakken/algemeen/software/STM32/STM32Basis/HAL/gemeenschappelijk/Libraries/BSP/STM32412G-Discovery/STM32412gDiscovery.cpp $
$Id: STM32412gDiscovery.cpp 3592 2019-09-05 15:56:28Z ewout $
************************************************************************/

#include <STM32412gDiscoveryGUI.h>

#include <GUI.h>
#include <WM.h>

SchermKalibratie::SchermKalibratie(STM32412gDiscovery &disc) : discovery(disc)
{

}

constexpr Int16 SchermKalibratie::kalibPunten[2];
//constexpr Int16 SchermKalibratie::pte = SchermKalibratie::kalibPunten[1]-SchermKalibratie::kalibPunten[0];


/**
  * @brief  Performs the TS calibration
  * @param  None
  * @retval Status (TS_OK = 0/ TS_ERROR = 1 / TS_TIMEOUT = 1 / TS_DEVICE_NOT_FOUND = 3)
  */
bool SchermKalibratie::kalibreer()
{
	auto ts_status = TS_OK;



	bool bkupgeladen=true;
	for (auto i=0;i<2;i++)
	{
		const UInt32 bkup = discovery.backupGeheugenLezen(i);
		if (0==bkup)
			bkupgeladen=false;
		else
		{
			meetpunten[i].x = static_cast<UInt16>(0xffff & bkup);
			meetpunten[i].y = static_cast<UInt16>(0xffff & (bkup>>16));
		}
	}

	if (false == bkupgeladen)
	{
		/* probeer te laden uit EEPROM */
		/* later implementeren */

		setHint();
		const auto ts_SizeX = BSP_LCD_GetXSize();
		const auto ts_SizeY = BSP_LCD_GetYSize();

		/* Start touchscreen internal calibration and configuration + start */
		ts_status = BSP_TS_InitEx(ts_SizeX, ts_SizeY, TS_ORIENTATION_LANDSCAPE);
		if (ts_status != TS_OK)
		{
			BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
			BSP_LCD_SetTextColor(LCD_COLOR_RED);
			BSP_LCD_DisplayStringAt(0, 240 - 95, (uint8_t *)"ERROR", CENTER_MODE);
			BSP_LCD_DisplayStringAt(0, 240 - 80, (uint8_t *)"Touchscreen cannot be calibrated", CENTER_MODE);
			if(ts_status == TS_ERROR)
			{
				BSP_LCD_DisplayStringAt(0, 240 - 65,
				                        (uint8_t *)"Touchscreen undefined error",
				                        CENTER_MODE);
			}
			else if(ts_status == TS_TIMEOUT)
			{
				BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 65,
				                        (uint8_t *)"Touchscreen Timeout",
				                        CENTER_MODE);
			}
			else
			{
				/* TS_DEVICE_NOT_FOUND */
				BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 65,
				                        (uint8_t *)"Touchscreen Not Found",
				                        CENTER_MODE);
			}
		}
		else
		{
			/* Get touch points for SW calibration processing */
			for (auto i = 0; i < 2; i++)
			{
				touchDetected=0;
				leesTouchWaarden(i, meetpunten[i].x, meetpunten[i].y);
				const UInt32 bkupwaarde = (meetpunten[i].x+(meetpunten[i].y<<16));
				discovery.backupGeheugenOpslaan(i,bkupwaarde);
			}
		}
	}

	return(ts_calibration_done = true);
}

/**
  * @brief  Display calibration hint
  * @param  None
  * @retval None
  */
void SchermKalibratie::setHint()
{
	/* Clear the LCD */
	BSP_LCD_Clear(LCD_COLOR_WHITE);

	/* Set Touchscreen Demo description */
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);

	BSP_LCD_SetFont(&Font20);
	BSP_LCD_DisplayStringAt(0, 100, (uint8_t *)"Touchscreen", CENTER_MODE);
	BSP_LCD_DisplayStringAt(0, 120, (uint8_t *)"Calibration", CENTER_MODE);
}


/**
  * @brief  Get Physical position
  * @param  LogX : logical X position
  * @param  LogY : logical Y position
  * @param  pPhysX : Physical X position
  * @param  pPhysY : Physical Y position
  * @retval None
  */
void SchermKalibratie::leesTouchWaarden(const Teller i,
                                        UInt16 &pPhysX,
                                        UInt16 &pPhysY)
{
	/* Draw the ring */
	const UInt16 LogX = kalibPunten[i],LogY = kalibPunten[i];
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillCircle(LogX, LogY, 20);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_FillCircle(LogX, LogY, 10);

	/* Wait until pressed state on the touch panel */
	wachtOpDrukStaat(1);

	/* Return as physical touch values the positions of first touch, even if double touched occurred */
	pPhysX = touchX[0];
	pPhysY = touchY[0];

	/* Wait until touch is released on touch panel */
	wachtOpDrukStaat(0);

	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_FillCircle(LogX, LogY, 20);
}

/**
  * @brief  TouchScreen_Calibration_WaitForPressedState : wait until a particular press/depress action
  *         The function is managing anti-rebound : that is the awaited state when detected
  *         needs to be stable for a sufficient time (timeout time), otherwise a new sense to search
  *         for awaited state is performed. When awaited state is found and state is stable for timeout
  *         duration, the function is exited.
  * @param  uint8_t Pressed :  Awaited pressed state
  *         - Await touch (single/multiple) detection if Pressed == 1
  *         - Await no touch detection if Pressed == 0
  * @retval None
  */
void SchermKalibratie::wachtOpDrukStaat(uint8_t Pressed)
{
	uint16_t TimeStart = 0;
	uint8_t  status = TS_OK;
	uint32_t exitFirstLevelWhileLoopReq = 0;  /* By default no exit request from first level while loop  */
	uint32_t exitSecondLevelWhileLoopReq = 0; /* By default no exit request from second level while loop */

	/* First level while loop entry */
	do
	{
		/* reset exit second level while loop in case it was set */
		exitSecondLevelWhileLoopReq = 0;

		/* Sense of touch state from touch IC until get the awaited state in parameter 'Pressed' */
		status = BSP_TS_GetState(this);
		if(status == TS_OK)
		{
			if (((Pressed == 0) && (touchDetected == 0)) ||
			    ((Pressed == 1) && ((touchDetected == 1) || (touchDetected == 2))))
			{
				/* Got awaited press state */
				/* Record in 'TimeStart' the time of awaited touch event for anti-rebound calculation */
				/* The state should persist for a minimum sufficient time */
				TimeStart = HAL_GetTick();

				/* Is state of the touch changing ? */
				/* Second level while loop entry */
				do
				{
					/* New sense of touch state from touch IC : to evaluate if state was stable */
					status = BSP_TS_GetState(this);
					if(status == TS_OK)
					{
						/* Is there a state change compared since having found the awaited state ? */
						if (((Pressed == 0) && ((touchDetected == 1) || (touchDetected == 2))) ||
						    ((Pressed == 1) && ((touchDetected == 0))))
						{
							/* Too rapid state change => anti-rebound management : restart first touch search */
							exitSecondLevelWhileLoopReq = 1; /* exit request from second level while loop */
						}
						else if ((HAL_GetTick() - 100) > TimeStart)
						{
							/* State have not changed for the timeout duration (stable touch for 100 ms) */
							/* This means the touch state is stable : can exit function */

							/* found valid touch, exit both while levels */
							exitSecondLevelWhileLoopReq = 1;
							exitFirstLevelWhileLoopReq  = 1;
						}

						/* Wait 10 ms before next sense of touch at next loop iteration */
						HAL_Delay(100);

					} /* of if(status == TS_OK) */
				}
				while (!exitSecondLevelWhileLoopReq);

			} /* of if (((Pressed == 0) && .... */

		} /* of if(status == TS_OK) */

		if(!exitFirstLevelWhileLoopReq)
		{
			/* Wait some time before next sense of touch at next loop iteration */
			HAL_Delay(100);
		}
	}
	while (!exitSecondLevelWhileLoopReq);
}


/**
  * @brief  Calibrate x position (to obtain X = calibrated(x))
  * @param  x : X position
  * @retval calibrated x
  */
uint16_t SchermKalibratie::kalibreerX(const uint16_t x) const
{
	const Int32 meetx = meetpunten[1].x-meetpunten[0].x;
	assert(0 != meetx);
	int32_t ret  = ((x - (meetpunten[0].x-kalibPunten[0]))*pte)/meetx;

	if (ret<0)
		ret=0;
	else if  (ret>240)
		ret=239;

	return (static_cast<uint16_t>(ret));
}

/**
  * @brief  Calibrate Y position
  * @param  y : Y position
  * @retval calibrated y
  */
uint16_t SchermKalibratie::kalibreerY(const uint16_t y) const
{
	const Int32 meety = meetpunten[1].y-meetpunten[0].y;
	assert(0 != meety);
	int32_t ret  = ((y - (meetpunten[0].y-kalibPunten[0]))*pte)/meety;

	if (ret<0)
		ret=0;
	else if  (ret>240)
		ret=239;

	return (static_cast<uint16_t>(ret));
}

/**check if the TS is calibrated
  * @param  None
* @retval calibration state (1 : calibrated / 0: no)
  */
bool SchermKalibratie::isGekalibreerd() const
{
	return (ts_calibration_done);
}

STM32412gDiscoveryGUIDriver::STM32412gDiscoveryGUIDriver(STM32412gDiscovery &disc) : SchermKalibratie(disc)
{
	/* Enable the CRC Module */
	__HAL_RCC_CRC_CLK_ENABLE();

	/***********************************************************/
	/* Init the STemWin GUI Library */
	GUI_X_InitOS();

	/* Activate the use of memory device feature */
	WM_SetCreateFlags(WM_CF_MEMDEV);
}

void STM32412gDiscoveryGUIDriver::postOSStartGUIInit()
{
	BSP_LCD_Init();
//	BSP_LCD_InitEx(LCD_ORIENTATION_LANDSCAPE_ROT180);

	GUI_Init();

	//GUI_SetOrientation(GUI_MIRROR_X | GUI_MIRROR_Y);
	/* Touchscreen initialization */
	const auto ts_SizeX = BSP_LCD_GetXSize();
	const auto ts_SizeY = BSP_LCD_GetYSize();

	/* Start touchscreen internal calibration and configuration + start */
	const auto ts_status = BSP_TS_InitEx(ts_SizeX, ts_SizeY,
	                                     TS_ORIENTATION_LANDSCAPE_ROT180);
	assert(TS_OK == ts_status);

	if (TS_OK == ts_status)
	{

		kalibreer(); /* voer de schermkalibratie uit. */

		/* zet de interrupt handling aan op pin GPIO G5 */
		BSP_TS_ITConfig();

	}

}

TS_StateTypeDef STM32412gDiscoveryGUIDriver::wachtOpSchermTouch() const
{
	/* haal de coordinaten van de touch op */
	TS_StateTypeDef tsStaat;
	TS_StatusTypeDef touchresult;
	do
	{
		/* wacht tot het scherm aangeraakt wordt */
		STM32412gDiscoveryGUIDriver::schermTouch.Take(portMAX_DELAY);
		touchresult = BSP_TS_GetState(&tsStaat);

	} while (TS_OK != touchresult);

	return(tsStaat);
}

CountingSemaphore STM32412gDiscoveryGUIDriver::schermTouch;
/**
  * @brief  Deze funktie handelt de touch interrupt van het scherm af.
  * @param  None
  * @retval None
  */
extern "C" void EXTI9_5_IRQHandler(void)
{
	const UInt32 exti5 = __HAL_GPIO_EXTI_GET_IT(TS_INT_PIN);
	HAL_GPIO_EXTI_IRQHandler(TS_INT_PIN);

	if (RESET != exti5)
	{
		BaseType_t wakkerGeworden;
		STM32412gDiscoveryGUIDriver::schermTouch.GiveFromISR(&wakkerGeworden);
		if (true == wakkerGeworden)
			portYIELD_FROM_ISR(wakkerGeworden);
	}
}


STM32412DiscoveryMetGUI::STM32412DiscoveryMetGUI(const UInt32 f) : STM32412gDiscovery(f),
                                                                   guiDriver(*this)
{

}
