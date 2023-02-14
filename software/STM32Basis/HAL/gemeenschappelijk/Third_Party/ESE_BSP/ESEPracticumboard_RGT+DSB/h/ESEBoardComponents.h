/*!
  @file
  API van de Board componenten op het ESE RGT+DSB Board:
  - TI ADS131A02 24 bit Sigma Delta A/D Converter
  - Maxim MAX 5136 16 bit D/A Converter

  @version $Rev: 4291 $
  @author $Author: ewout $

  @copyright Copyright 2009-2019 HAN Embedded Systems Engineering

  $Id: ESEBoardComponents.h 4291 2021-06-21 19:27:46Z ewout $
************************************************************************/

#ifndef HANESE_ESEBoardComponents_H_
#define HANESE_ESEBoardComponents_H_

#include <algdef.h>
#include <dataPakket.h>

#ifdef  FreeRTOSGebruikt
#include <semaphore.hpp>
#endif

class STM32BoardController;  /* voorwaartse deklaratie/forward declaration */

/*! @class Deze klasse vertegenwoordigt een driver voor de TI ADS131A02 ADC.
 *  Voor meer informatie, zie : http://www.ti.com/product/ADS131A02.
 *  De driver is in het Nederlands gescrheven, met Engelse uitbreiding en vertaalde funkties.
 *  English :
 *  This class represents a driver for the TI ADS131A02 ADC.
  * For more information, see: http://www.ti.com/product/ADS131A02.
  * The driver is written in Dutch, with English expansion and translated functions.
 */
class ADS131A02
{
public:
	explicit ADS131A02(STM32BoardController &);

	void reset();

	/*! @enum This typedef defines the clock in divider to make from clk in the iclk. */
	enum class ICLK : UInt8
	{
		ICLK2=0b001, /* deling door/division by 2 */
		ICLK4=0b010,
		ICLK6=0b011,
		ICLK8=0b100,
		ICLK10=0b101,
		ICLK12=0b110,
		ICLK14=0b111, /* deling door/division by 14 */
		LaatsteICLK
	};

	/*! @enum Deze typedef definieert de klok in divider om uit iclk in de modulatieklok te maken te maken.
	 * This typedef defines the clock in divider to make from iClk in the modulation clock.*/
	enum class FMOD : UInt8
	{
		FMOD2=0b001,  /* deling door/division by 2 */
		FMOD4=0b010,
		FMOD6=0b011,
		FMOD8=0b100,
		FMOD10=0b101,
		FMOD12=0b110,
		FMOD14=0b111, /* deling door/division by 14 */
		LaatsteFMOD
	} ;

	/*! @enum Deze enumeratie definieert de klok in divider om uit modulatieklok de OutputDataRate te maken.
	 * This enumeration defines the clock in divider to create the OutputDataRate from the modulation clock.*/
	enum class ODR : UInt8
	{
		ODR32=0b1111,  /* deling door/division by 32 */
		ODR48=0b1110,
		ODR64=0b1101,
		ODR96=0b1100,
		ODR128=0b1011,
		ODR192=0b1010,
		ODR200=0b1001,
		ODR256=0b1000,
		ODR384=0b0111,
		ODR400=0b0110,
		ODR512=0b0101,
		ODR768=0b0100,
		ODR800=0b0011,
		ODR1024=0b0010,
		ODR2048=0b0001,
		ODR4096=0b0000, /* deling door/division by 4096 */
		LaatsteODR=0b10000
	};

	using Spanning = WFT;

	/*! @brief Zet de sampling frequentie. De hoofdklok van de ADS131A02 is 16,384 MHz.
	 * @param De CLKIN ==> ICLK deler Met deze parameter wordt de hoofdklok gedeeld en aangeboden als de Interne Klok ICLK.
	 * @param De ICLK ==> Fmodulatie deler. Met deze parameter wordt de ICLK gedeeld en aangeboden als de Modulator frequentie Fmodulatie.
	 * @param De Fmodulatie ==> Output Data Rate deler. Na de Modulatie wordt de datastroom met deze paramter ge-downsampled naar de output rate.
	 * @note Zie ADS131A02::zelfTest() voor voorbeelden van frequenties. */
	void zetSampFreq(const ICLK, const FMOD, const ODR);

	/*! @brief na instelling van de sampling frequentie, start de ADC */
	/* After setting the sampling frequency, the ADC * starts */
	void start() const;

	/*! @brief Zet de sampling frequentie en start de ADC conversie. De hoofdklok van de ADS131A02 is 16,384 MHz.
	 * @param De CLKIN ==> ICLK deler Met deze parameter wordt de hoofdklok gedeeld en aangeboden als de Interne Klok ICLK.
	 * @param De ICLK ==> Fmodulatie deler. Met deze parameter wordt de ICLK gedeeld en aangeboden als de Modulator frequentie Fmodulatie.
	 * @param De Fmodulatie ==> Output Data Rate deler. Na de Modulatie wordt de datastroom met deze paramter ge-downsampled naar de output rate.
	 * @note Zie ADS131A02::zelfTest() voor voorbeelden van frequenties. */
	/*! @brief Set the sampling frequency and start the ADC conversion. The main clock of the ADS131A02 is 16.384 MHz.
	* @param The CLKIN ==> ICLK divisor With this parameter the main clock is shared and presented as the Internal Clock ICLK.
	* @param The ICLK ==> Fmodulation divisor. With this parameter the ICLK is shared and offered as the Modulator frequency Fmodulation.
	* @param The Fmodulation ==> Output Data Rate divisor. After the Modulation, the data stream is downsampled with this parameter to the output rate.
	* @note See ADS131A02 :: selfTest () for examples of frequencies. */
	void start(const ICLK, const FMOD, const ODR);

	/*! @brief pauzeer de ADC, maar laat de instellingen intact. */
	void pauze();

	/*! @brief Hervat (na een pauze) de ADC conversie */
	void hervat();

	/*! @brief stop de ADC conversie */
	/*! @brief stop the ADC conversion */
	void stop();

	/*! @brief wacht tot data beschikbaar is.
	 * Wait for data to be available. */
	void wachtOpDataReady() const;

	/*! @brief Wacht (statisch toegankelijk) tot de ADC heeft aangegeven dat er data beschikbaar is. */
	/*! @brief Wait (statically accessible) until the ADC has indicated that data is available. */
	static void dataReady();

	/*! @enum Deze enum geeft aan welk ADC kanaal geselekteerd wordt.
	 * This enum indicates which ADC channel is selected. */
	enum class Kanaal
	{
		K1=0,
		K2,
	};

	static constexpr auto AantalKanalen = 1+static_cast<Teller>(Kanaal::K2);

	/*! @brief ADC data is verpakt in een pakket van vastgelegde grootte.
	 *  ADC Data is stored in an array of fixed size. */
	using ADCWaarde = UInt32;
	using ADCData = FixedDataPakket<ADCWaarde,AantalKanalen> ;

	/*! @brief haal de A/D conversiewaarden op uit de ADC.
	 *  @note de conversieresultaten worden in deze klasse opgeslagen */
	void laadConversieData();

	/*! @brief haal de A/D conversiewaarden op uit de ADC.
	 *  @param Deze container bevat na aanroep van de funktie de twee conversiewaarden.
	 *  @note de conversieresultaten worden ook in deze klasse opgeslagen*/
	void laadConversieData(ADCData &);

	/*! @brief Deze overloading geeft de conversiedata
 	 * @param deze index geeft het gewenste kanaal aan. */
	/*! @brief This overloading gives the conversion data
	* @param this index indicates the desired channel. */
	ADCWaarde operator [] (const Kanaal) const;

	/*! @brief converteer een intern opgeslagen kanaalconversiewaarde naar float formaat.
	 * @param Het kanaal dat moet worden gekonverteerd. */
	Spanning konverteer(const Kanaal) const;

#ifdef InterfaceTaalEnglish

	/*! @brief Set the sampling frequency. The main clock of the ADS131A02 is 16,384 MHz.
	* @param The CLKIN ==> ICLK divisor With this parameter the main clock is shared and presented as the Internal Clock ICLK.
	* @param The ICLK ==> Fmodulation divisor. With this parameter the ICLK is shared and offered as the Modulator frequency Fmodulation.
	* @param The Fmodulation ==> Output Data Rate divisor. After the Modulation, the data stream is downsampled with this parameter to the output rate.
	* @note See ADS131A02 :: selfTest () for examples of frequencies. */
	void setSamplingFreq(const ICLK iclk, const FMOD fmod1, const ODR odr)
	{
		zetSampFreq(iclk,fmod1,odr);
	};

	/*! @enum This enum indicates which ADC channel is selected. */
	using Channel = Kanaal;

	using Voltage = Spanning;

	/*! @brief pause the ADC, but leave the settings intact. */
	inline void pause()
	{
		pauze();
	};

	/*! @brief Resumes (after a break) the ADC conversion */
	inline void resume()
	{
		hervat();
	};

	/*! @brief Obtain the A/D conversion values from the ADC.
	* @note the conversion results are saved in this class */
	inline void loadConversionData()
	{
		laadConversieData();
	}

	/*! @brief get the A / D conversion values from the ADC.
	* @param This container contains the two conversion values after calling the function.
	* @note the conversion results are also saved in this class */
	inline void loadConversionData(ADCData &adcDataPackage)
	{
		laadConversieData(adcDataPackage);
	};

	/*! @brief convert an internally saved channel conversion value to float format.
	 * @param The channel that must be converted. */
	inline Voltage convert(const Channel channel) const
	{
		const auto voltage = konverteer(channel);
		return(voltage);
	};

#endif

	/*! @brief Doe een zelftest. */
	void zelfTest();

private:
	STM32BoardController &controller;

	/* commandos */
	typedef enum
	{
		NULLCmd = 0x0000,
		ResetCmd = 0x1100,
		StdByCmd = 0x2200,
		WakeupCmd = 0x3300,
		LockCmd = 0x5505,
		UnlockCmd = 0x5506,
		RRegBasisCmd = (1 << 5),
		WRegBasisCmd = (1 << 6),
		WRegsBasisCmd = (3 << 5)
	} Commando;

	/*! @brief stuur een commando naar de ADS131A02 */
	void stuurCommando(const ADS131A02::Commando,
	                   const UInt16) const;

	typedef enum
	{
		ASysCfgReg = 0x0b,
		DSysCfgReg = 0x0c,
		CLK1Reg = 0x0d,
		CLK2Reg = 0x0e,
		ADCENAReg = 0x0f,
		ADC1Reg = 0x11,
		ADC2Reg = 0x12,
		ADC3Reg = 0x13,
		ADC4Reg = 0x14,
		EindeRegisters=0x15
	} RegisterAdres;

	UInt8 leesRegister(const RegisterAdres reg) const;
	void schrijfRegister(const RegisterAdres reg, const UInt8) const;

	ADCData adcData;

#ifndef FreeRTOSGebruikt
	static volatile bool dataIsReady;
#else
	static cpp_freertos::CountingSemaphore dataIsReady;
#endif
	bool sampFreqGezet;
};


/*! @class Deze klasse vertegenwoordigt een driver voor de MAX5136 D/A converter.
 * Zie https://www.maximintegrated.com/en/products/analog/data-converters/digital-to-analog-converters/MAX5136.html
 * De driver is in het Nederlands gescrheven, met Engelse uitbreiding en vertaalde funkties.
  English : This class represents a driver for the MAX5136 D / A converter.
* See https://www.maximintegrated.com/en/products/analog/data-converters/digital-to-analog-converters/MAX5136.html.
 * The driver is written in Dutch, with English expansion and translated functions. */
class MAX5136
{
public:
	explicit MAX5136(STM32BoardController &);

	void reset();

	enum class Kanaal : UInt8
	{
		K1=0b1,
		K2=0b10,
		AlleKanalen=0b11
	};

	using DACWaarde = UInt16;
	using Spanning = WFT;

	/*! @brief start de D/A conversie van een kanaal. */
	/*! @brief starts the D / A conversion of a channel. */
	void start(const Kanaal);

	/* @brief stop de D/A conversie van een kanaal (naar slaapstand) */
	/* @brief stops the D / A conversion of a channel (to sleep mode) */
	void stop(const Kanaal);

	/*! @brief Zet een van beide kanalen op een DAC waarde.
	 * @param de 16 bit DAC waarde (0..0xffff) waarbij :
	 * 0 --> 0V
	 * 0xffff (65535) --> 5V */
	/*! @brief Set one of the two channels to a DAC value.
	* @param the 16 bit DAC value (0..0xffff) where:
		* 0 -> 0V
	* 0xffff (65535) -> 5V */
	void zetKanaal(const Kanaal,const DACWaarde );

	/*! @brief Zet een van beide kanalen op een DAC waarde.
	 * @param de 16 bit DAC waarde (0..0xffff) waarbij :
	 * 0 --> 0V
	 * 0xffff (65535) --> 5V
	 * @param Zet een offset value (handig bij een bandfilter uitvoer). */
	/*! @brief Set one of the two channels to a DAC value.
	* @param the 16 bit DAC value (0..0xffff) where:
		* 0 -> 0V
	* 0xffff (65535) -> 5V */
	void zetKanaal(const Kanaal,const DACWaarde, const DACWaarde );


	/*! @brief Zet een van beide kanalen op een spanning.
 	 * @param de gewenste analoge spanning (0,0 ... 5,0 V).
 	 * @param autoBegrens : kap af indoen buiten het 0..5V bereik.*/
	/*! @brief Switch on one of the two channels.
	* @param the desired analog voltage (0.0 ... 5.0 V).
	* @param auto Limit: cut off outside the 0..5V range. */
	void zetSpanning(const Kanaal,
	                 const Spanning,
	                 const bool autoBegrens=true);

	/*! @brief Laad een DAC waarde in het DAC register, maar zet dit nog niet op de DAC uitgang */
	void laadKanaal(const Kanaal, const DACWaarde);

	/*! @brief Zet beide kanalen tegelijkertijd. */
	void zetKanaal(const DACWaarde,const DACWaarde);

	/*! @brief Zet een spanningen op beide kanalen tegelijkertijd.
	 * @param spanning op kanaal 1.
	 * @param spanning op kanaal 2. */
	void zetSpanning(const Spanning, const Spanning);

	/*! @brief Een hulpfunktie : converteer een DAC waarde naar de overeenkomstige spanning.
	 * @param De DAC waarde.
	 * @return De spanning. */
	static WFT dacSpanning(const DACWaarde );

	using DACData = FixedDataPakket<UInt8,3> ;

#ifdef InterfaceTaalEnglish
	/*! @enum This enum indicates which DAC channel is selected. */
	using Channel = Kanaal;
	using DACValue = DACWaarde;
	using Voltage = Spanning;

	/*! @brief Set one of the two channels to a DAC value.
	 * @param the 16 bit DAC value (0..0xffff) where:
	 * 0 -> 0V
	 * 0xffff (65535) -> 5V */
	void setChannel(const Channel channel,const DACValue dacValue)
	{
		zetKanaal(channel,dacValue);
	};

	/*! @brief Set the voltage on one of the two channels.
	* @param the desired analog voltage (0.0 ... 5.0 V).
	* @param auto Limit: cut off outside the 0..5V range. */
	void setVoltage(const Channel channel,
	                const Voltage voltage,
	                const bool autoLimit=true)
	{
		zetSpanning(channel,voltage,autoLimit);
	};

	/*! @brief Load a DAC value in the DAC register, but do not yet set this to the DAC output */
	void loadChannel(const Channel channel, const DACValue dacValue)
	{
		laadKanaal(channel,dacValue);
	};

	/*! @brief Load values in both channels at the same time. */
	void setChannel(const DACValue value1,const DACValue value2)
	{
		zetKanaal(value1,value2);
	};

	/*! @brief Load voltages in both channels at the same time. */
	void setVoltage(const Voltage voltage1, const Voltage voltage2 )
	{
		zetSpanning(voltage1,voltage2);
	};

	/*! @brief An auxiliary function: convert a DAC value to the corresponding voltage.
	* @param The DAC value.
	* @return The voltage. */
	static WFT dacVoltage(const DACValue dacValue)
	{
		const auto voltage = dacSpanning(dacValue);
		return(voltage);
	};

#endif

	/*! @brief Doe een zelftest. */
	void zelfTest();

private:

	/*! @brief converteer tussen 0,0 ... 5,0V bereik en de DAC waarde.
	 * @param de gevraagde spanning in V
	 * @return de benodigde DAC waarde. */
	/*! @brief convert between 0.0 ... 5.0V range and the DAC value.
	* @param the requested voltage in V
	* @return the required DAC value. */
	static DACWaarde convert(const WFT );

	STM32BoardController &controller;

	typedef enum
	{
		NOP=0b0,
		LDAC,
		CLR,
		PwrCtrl,
		Linearity,
		Write=0b010000,
		WriteThrough=0b110000,
		LaatsteRegister
	} Regs;



};

/*! @class deze klasse vertegenwoordigt een driver voor het zenden en ontvangen van data via de FT232 IC
 * dat op het board aanwezig is. */
/*! English : this class represents a driver for sending and receiving data via the FT232 IC
	* that is present on the board. */
class FTDI232BQ
{
public:
	explicit FTDI232BQ(STM32BoardController &);

	void reset();

	using FTDIData = DataPakket<UInt8> ;

	/*! @brief verstuur een enkele byte */
	void verstuurByte(const UInt8);
	/*! @brief verstuur een pakket met data */
	void verstuur(const FTDIData &);

	/*! @brief ontvang een enkele byte */
	UInt8 ontvangByte();
	/*! @brief ontvang een pakket data */
	void ontvang(const FTDIData &);

	/*! @brief doe een zelftest. */
	void zelfTest();

private:

	STM32BoardController &controller;
};

#endif // Perifs
