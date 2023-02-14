
/*!
  @file
  Implementatie om de Digitale Proces Processor aan te sturen

  @version $Rev: 3445 $
  @author $Author: ewout $

  @copyright Copyright 2002-2017 HAN Embedded Systems Engineering

  $Id: DigProcesAansturing.cpp 3445 2019-03-14 16:19:08Z ewout $
************************************************************************/

#include <DigProcesAansturing.h>

void DigProcesAansturing::zetProcesControle(const DigProcesAansturing::ProcesControle controle)
{
	this->operator [] (1) = static_cast<UInt8>(controle);
}

void DigProcesAansturing::zetProcesType(const DigProcesAansturing::ProcesType type)
{
	this->operator [] (1) = static_cast<UInt8>(type);
}

void DigProcesAansturing::zetKommando(const DigProcesAansturing::ProcesCmd cmd)
{
	this->operator [] (0) = static_cast<UInt8>(cmd);
}

void DigProcesAansturing::zetSampFreq(const UInt32 freq)
{
	zetKommando(ProcesCmd::ZetSampFreq);

	/* kopieer de parameter naar de array */
	UInt8 * const dstPtr = &(this->operator [] (1));
	memcpy(dstPtr, reinterpret_cast<UInt8 *>(const_cast<UInt32 *>(&freq)),sizeof(UInt32));
}

void DigProcesAansturing::zetNuldeOrdeParams(const float param)
{
	zetKommando(ProcesCmd::ZetNuldeOrdeParams);

	/* kopieer de parameter naar de array */
	UInt8 * const dstPtr = &(this->operator [] (1));
	memcpy(dstPtr, reinterpret_cast<UInt8 *>(const_cast<float *>(&param)),sizeof(float));
}

void DigProcesAansturing::zetEersteOrdeParams(const float kpparam, const float tauparam)
{
	zetKommando(ProcesCmd::ZetEersteOrdeParams);

	/* kopieer de paramaters naar de array */
	UInt8 *dstPtr = &(this->operator [] (1));
	memcpy(dstPtr, reinterpret_cast<UInt8 *>(const_cast<float *>(&kpparam)),sizeof(float));
	dstPtr += sizeof(float);
	memcpy(dstPtr, reinterpret_cast<UInt8 *>(const_cast<float *>(&tauparam)),sizeof(float));
}

void DigProcesAansturing::zetTweedeOrdeParams(const float kpparam, const float omega0param, const float betaparam)
{
	zetKommando(ProcesCmd::ZetTweedeOrdeParams);

	/* kopieer de paramaters naar de array */
	UInt8 *dstPtr = &(this->operator [] (1));
	memcpy(dstPtr, reinterpret_cast<UInt8 *>(const_cast<float *>(&kpparam)),sizeof(float));
	dstPtr += sizeof(float);
	memcpy(dstPtr, reinterpret_cast<UInt8 *>(const_cast<float *>(&omega0param)),sizeof(float));
	dstPtr += sizeof(float);
	memcpy(dstPtr, reinterpret_cast<UInt8 *>(const_cast<float *>(&betaparam)),sizeof(float));
}

void DigProcesAansturing::zetCustomOverdracht(const DataPakket<float> &custom)
{
	zetKommando(ProcesCmd::ZetCustomOverdrachtFunktie);

	const UInt16 customgrootte = static_cast<UInt16>(custom.geefGrootte());

	UInt8 *dstPtr = &(this->operator [] (1));
	memcpy(dstPtr, reinterpret_cast<UInt8 *>(const_cast<UInt16 *>(&customgrootte)),sizeof(UInt16));
	dstPtr =  &(this->operator [] (cmdLengte));
	UInt8 * const srcPtr = reinterpret_cast<UInt8 *>(const_cast<float *>(&(custom[0])));

	memcpy(dstPtr, srcPtr, customgrootte*sizeof(float));
}

DataPakket<UInt8> DigProcesAansturing::maakCommandoPakket() const
{
	const DataPakket<UInt8> pakje(&(this->operator [] (0)),cmdLengte);
	return(pakje);
}

DataPakket<UInt8> DigProcesAansturing::maakCustomPakket() const
{
	const DataPakket<UInt8> pakje(&(this->operator [] (cmdLengte)),MaxOverdrachtFunktieGrootte);
	return(pakje);
}


DigProcesAansturing::ProcesCmd DigProcesAansturing::identificeerCommando() const
{
	const auto cmd = static_cast<ProcesCmd>(this->operator [] (0));

	return(cmd);
}

UInt32 DigProcesAansturing::geefSampFreq() const
{
	assert((identificeerCommando() == ProcesCmd::ZetSampFreq));

	UInt32 fs;
	UInt8 const * const srcPtr = &(this->operator [] (1));
	memcpy(reinterpret_cast<UInt8 *>(&fs),srcPtr,sizeof(UInt32));
	return(fs);
}

DigProcesAansturing::ProcesControle DigProcesAansturing::geefProcesControle() const
{
	const auto controle = static_cast<ProcesControle>(this->operator [] (1));
	return(controle);
}

DigProcesAansturing::ProcesType DigProcesAansturing::geefProcestype() const
{
	const auto type = static_cast<ProcesType>(this->operator [] (1));
	return(type);
}


float DigProcesAansturing::geefKp() const
{
	assert((identificeerCommando() >= ProcesCmd::ZetNuldeOrdeParams) &&
				   (identificeerCommando() <= ProcesCmd::ZetTweedeOrdeParams));
	float kpparam;
	auto srcPtr = &(this->operator [] (1));
	memcpy(reinterpret_cast<UInt8 *>(&kpparam),srcPtr,sizeof(float));
	return(kpparam);
}

float DigProcesAansturing::geefTau() const
{
	assert((identificeerCommando() == ProcesCmd::ZetEersteOrdeParams));

	float tauparam;
	auto srcPtr = &(this->operator [] (1+sizeof(float)));
	memcpy(reinterpret_cast<UInt8 *>(&tauparam),srcPtr,sizeof(float));
	return(tauparam);
}

float DigProcesAansturing::geefOmega0() const
{
	assert((identificeerCommando() == ProcesCmd::ZetTweedeOrdeParams));

	float omega0param;
	auto srcPtr = &(this->operator [] (1+sizeof(float)));
	memcpy(reinterpret_cast<UInt8 *>(&omega0param),srcPtr,sizeof(float));
	return(omega0param);
}

float DigProcesAansturing::geefBeta() const
{
	assert((identificeerCommando() == ProcesCmd::ZetTweedeOrdeParams));

	float betaparam;
	auto srcPtr = &(this->operator [] (1+(2*sizeof(float))));
	memcpy(reinterpret_cast<UInt8 *>(&betaparam),srcPtr,sizeof(float));
	return(betaparam);
}








