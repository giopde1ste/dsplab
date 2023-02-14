/*!
  @file
  API om de Digitale Proces Processor aan te sturen

  @version $Rev: 3445 $
  @author $Author: ewout $

  @copyright Copyright 2009-2019 ir drs E.J Boks, HAN Embedded Systems Engineering

  $Id: DigProcesAansturing.h 3445 2019-03-14 16:19:08Z ewout $
************************************************************************/

#ifndef DigProcesAansturing_H
#define DigProcesAansturing_H

#include <algdef.h>
#include <dataPakket.h>

/* Commando opbouw : ProcesCmd + 3*4 data bytes data + crc = 14 bytes*/
static constexpr auto cmdLengte = 1 + (3*4) + 1;
/* De overdracht kan maximaal 512/4 = 128 floating point impulsresponisiecoefficienten overbrengen. */
static constexpr auto MaxOverdrachtFunktieGrootte=512;

static constexpr auto TotaalPakketGrootte = cmdLengte+MaxOverdrachtFunktieGrootte;

class DigProcesAansturing : public FixedDataPakket<UInt8,TotaalPakketGrootte>
{
public:
	DigProcesAansturing() = default;
	
	/**************** Verzendingen ***************/
	enum class ProcesControle : UInt8
	{
		Stop=0,
		Start,
		Pauze
	};

	void zetProcesControle(const ProcesControle );
	
	enum class ProcesType : UInt8
	{
		NuldeOrde=0,
		EersteOrde,
		TweedeOrde,
		CustomOverdrachtFunktie
	} ;

	void zetProcesType(const ProcesType);

	void zetSampFreq(const UInt32 );

	/*! @brief zet de Kp van het nuldeordeproces.
	 * @param Kp*/
	void zetNuldeOrdeParams(const float );

	/*! @brief zet de Kp en de tau van het eersteordeproces.
	 * @param Kp.
	 * @param tau. */
	void zetEersteOrdeParams(const float,const float);

	/*! @brief zet de Kp, omega0 en beta van het tweedeordeproces.
	 * @param Kp.
	 * @param omega0.
	 * @param beta */
	void zetTweedeOrdeParams(const float,const float,const float);

	/*! @brief zet de impulsresponsie van de custom overdrachtrfunktie.
	 * @param een reeks floating point waardem h[n] */
	void zetCustomOverdracht(const DataPakket<float> &);

	DataPakket<UInt8> maakCommandoPakket() const;
	DataPakket<UInt8> maakCustomPakket() const;

	/****** Ontvangst *****************/
	enum class ProcesCmd : UInt8
	{
		ProcesControle=0,
		ProcesType,
		ZetSampFreq,
		ZetNuldeOrdeParams,
		ZetEersteOrdeParams,
		ZetTweedeOrdeParams,
		ZetCustomOverdrachtLengte,
		ZetCustomOverdrachtFunktie
	} ;
	

	
	/*! @brief bij binnenkomend : identificeer het commando */
	ProcesCmd identificeerCommando() const;

	/*! @brief haal de sampling frequentie op */
	UInt32 geefSampFreq() const;
	ProcesControle geefProcesControle() const;
	ProcesType geefProcestype() const ;

	/*! @brief haal de parameters van de processen op */
	float geefKp() const;
	float geefTau() const;
	float geefOmega0() const;
	float geefBeta() const;

	DataPakket<UInt8> geefCustomOverdracht() const;

protected:
	void zetKommando(const ProcesCmd );
	
private:

};


#endif /* DigProcesAansturing_H */
