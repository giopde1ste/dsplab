/*!
@file
API van de c++ schil voor gebruik van de FreeRTOS
@version $Rev: 2892 $
@author $Author: ewout $

@copyright Copyright 2017-2018 Hogeschool van Arnhem en Nijmegen, ir drs E.J Boks

$URL: http://svn.hanese.nl/docenten/svn/ewout/vakken/algemeen/software/STM32/STM32Basis/HAL/gemeenschappelijk/Third_Party/ESE_BSP/ESEPracticumboard_RGT+DSB/h/ESEBoardController.h $
$Id: ESEBoardController.h 2892 2018-02-27 19:33:20Z ewout $
************************************************************************/

#ifndef ESE_BSP_STDTaak_H
#define ESE_BSP_STDTaak_H

#include <thread.hpp>
#include <queue.hpp>

#include <assert.h>
#include <semaphore.hpp>
#include <algdef.h>

using namespace cpp_freertos;

/*! @enum De taken prioriteiten.
 * @note FreeRTOS prioriteiten zijn van laag (lager nummer) naar hoog (hoger nummer),
 * dit in tegenstelling tot bijvoorbeeld Micrium UCOSII/III */
enum class TaakPrio
{
	Taak1=2,  /* lowest prio */
	Taak2,
	Taak3,
	Taak4,
	//Taak5, /* highest prio */
	Onbekend
};

enum class MsgType
{
	TouchGebeurd,
	ADCGebeurd,
	KnopGedrukt,
	StartStopGedrukt,
	SignaalKeuzeVeranderd,
	SignaalAmplitudeVeranderd,
	VerbindingGemaakt,
	VerbindingLosgemaakt,
	Onbekend
};

template<const unsigned int Grootte>
class StdTaakMsg
{
public:
	/*! @brief voor ontvangst */
	StdTaakMsg() : zender(TaakPrio::Onbekend),
	               ontvanger(TaakPrio::Onbekend),
	               type(MsgType::Onbekend)
	{

	};

	/*! @brief voor verzenden */
	StdTaakMsg(const TaakPrio z,
	           const TaakPrio o,
	           const MsgType t) : zender(z),ontvanger(o),type(t)
	{

	};

	UInt32 operator [] (const Teller index) const
	{
		assert(index < Grootte);
		return(vracht[index]);
	};

	void zetVracht(const Teller index,const UInt32 waarde)
	{
		assert(index < Grootte);
		vracht[index] = waarde;
	};

	MsgType geefType() const
	{
		return(type);
	};

	TaakPrio geefOntvanger() const
	{
		return(ontvanger);
	};

	TaakPrio geefZender() const
	{
		return(zender);
	};

private:

	TaakPrio zender,ontvanger;
	MsgType type;
	UInt32 vracht[Grootte];
};


class StdTaak : public Thread
{
public:
	StdTaak(const std::string &,
	        const UInt16,
	        const TaakPrio);


	TaakPrio geefPrioriteit();

	static constexpr auto VrachtGrootte=16;
	using StdMsg = StdTaakMsg<VrachtGrootte>;
	static constexpr auto QueueAantal = 24;
	static constexpr auto QueueMsgGrootte = sizeof(StdMsg);

	void stuurBericht(const StdMsg &);

	/*! @brief Wacht een aantal millisekonden.
	 * @param Het aantal ms.
	 * @return Is het gelukt of niet ? */
	FoutCode wacht(const Teller);

protected:

	static void registreer(StdTaak * const, const TaakPrio);
	static StdTaak * geefTaak(const TaakPrio);

	bool isBerichtAanwezig();

	/*! @brief Wacht op een bericht.
	 * @param msecs  De time-out tijd
	 * @return Is er een berich (Ok) of niet (Waarschuwing) */
	FoutCode wachtOpBericht(const UInt16 msecs);
	
	/*! @brief Geef het type van het laatst ontvangen bericht. */
	MsgType geefType() const;

	/*! @brief Geef het volledige bericht. */
	StdMsg geefBericht() const;

	/*! @brief converteer het een gegeven aantal millisekonden naar FreeRTOS ticks */
	static UInt32 aantalTicks(const UInt32 );
	
private:

	TaakPrio geefZender() const;
	TaakPrio geefOntvanger() const;

	StdMsg nieuwBericht;
	Deque brievenbus;
	
	using TakenRegister = StdTaak *;
	
	static constexpr Teller AantalTaken = static_cast<Teller>(TaakPrio::Onbekend)-static_cast<Teller>(TaakPrio::Taak1);
	static TakenRegister takenIndex[AantalTaken];
};




#endif // ESE_BSP_STDTaak_H
