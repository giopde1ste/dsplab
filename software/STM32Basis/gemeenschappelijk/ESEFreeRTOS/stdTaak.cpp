/*!
@file
Implementatie van de c++ schil voor gebruik van de FreeRTOS
@version $Rev: 2892 $
@author $Author: ewout $

@copyright Copyright 2017-2018 Hogeschool van Arnhem en Nijmegen, ir drs E.J Boks

$URL: http://svn.hanese.nl/docenten/svn/ewout/vakken/algemeen/software/STM32/STM32Basis/HAL/gemeenschappelijk/Third_Party/ESE_BSP/ESEPracticumboard_RGT+DSB/h/ESEBoardController.h $
$Id: ESEBoardController.h 2892 2018-02-27 19:33:20Z ewout $
************************************************************************/
#include <algdef.h>
#include <stdTaak.h>

StdTaak::StdTaak(const std::string &naam,
                 const UInt16 stackGrootte,
                 const TaakPrio prioriteit) : Thread(naam, stackGrootte,
													 static_cast<UInt32>(prioriteit)),
                                              brievenbus(QueueAantal,
														 QueueMsgGrootte)
{
	registreer(this,prioriteit);
}

TaakPrio StdTaak::geefPrioriteit()
{
	const auto prio = static_cast<TaakPrio>(GetPriority());
	return(prio);
}

StdTaak *StdTaak::takenIndex[StdTaak::AantalTaken];

void StdTaak::registreer(StdTaak *const tp, const TaakPrio prio)
{
	const auto index = static_cast<Teller>(prio)-2;
	assert(index < AantalTaken);
	takenIndex[index] = tp;
}

StdTaak *StdTaak::geefTaak(const TaakPrio prio)
{
	const auto index = static_cast<Teller>(prio)-2;
	assert(index < AantalTaken);
	auto * const taakPtr = takenIndex[index];
	assert(nullptr != taakPtr);
	return(taakPtr);
}

void StdTaak::stuurBericht(const StdTaak::StdMsg &msg)
{
	const auto taakID = msg.geefOntvanger();
	auto * const ontvanger = geefTaak(taakID);
	ontvanger->brievenbus.Enqueue(const_cast<StdMsg *>(&msg));
}

FoutCode StdTaak::wachtOpBericht(const UInt16 msecs)
{
	const auto aantalWachtTicks = ((0 != msecs) ? aantalTicks(msecs) : portMAX_DELAY );

	const bool nieuwAanwezig = brievenbus.Dequeue(&nieuwBericht,aantalWachtTicks);

	return((true==nieuwAanwezig) ? FoutCode::Ok : FoutCode::Waarschuwing);
}

UInt32 StdTaak::aantalTicks(const UInt32 msecs)
{
	assert(0 != configTICK_RATE_HZ);
	static constexpr auto ticksInMsecsPeriode = 1000/configTICK_RATE_HZ;
	assert(0 != ticksInMsecsPeriode);

	const auto periode = msecs/ticksInMsecsPeriode;

	const auto uit = ((periode != 0) ? periode : ticksInMsecsPeriode);

	return(uit);
}

MsgType StdTaak::geefType() const
{
	const auto antwoord = nieuwBericht.geefType();
	return(antwoord);
}

TaakPrio StdTaak::geefZender() const
{
	const auto antwoord = nieuwBericht.geefZender();
	return(antwoord);
}

TaakPrio StdTaak::geefOntvanger() const
{
	const auto antwoord = nieuwBericht.geefOntvanger();
	return(antwoord);
}

StdTaak::StdMsg StdTaak::geefBericht() const
{
	/* maak een kopie. */
	const auto kopie(nieuwBericht);
	return(kopie);
}

bool StdTaak::isBerichtAanwezig()
{
	const bool aanwezig = (false == brievenbus.IsEmpty());
	return(aanwezig);
}

FoutCode StdTaak::wacht(const Teller msecs)
{
	/* kap de vertraging af op het minimum.  */
	const auto rauweVertraging = msecs / portTICK_PERIOD_MS;
	const auto vertraging = (rauweVertraging > 1) ? rauweVertraging : 1;

	Delay(vertraging);

	return (FC::Ok);
}
