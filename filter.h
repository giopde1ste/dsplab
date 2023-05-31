/* Dit is een C/C++ Header bestand                                                                        */
/* Opdracht 4 Digitale Signaalbewerking/Digital Signal Processing  ESEDSP                                 */
/* Copyright 2006-2023 Opleiding Embedded Systems Engineering, Hogeschool van Arnhem en Nijmegen          */
/* Tijdstempel/Time Stamp : 2023-05-30 15:08:58 */
#ifndef FilterCoeffsExport_ESE_H
#define FilterCoeffsExport_ESE_H
/* Onderstaande coefficienten zijn voor een bandpass Q8 fixed-point implementatie FIR filter */
/* Start vd band : 0.000000*PI (0 Hz @ fs=100 Hz) */
/* Eind vd band : 0.080000*PI (8 Hz @ fs=100 Hz) */
/* Taps : 21 */



typedef signed short Int16;

static constexpr auto AantalTaps=21;
static constexpr auto SchaalFaktor=256;

const Int16 filterFixedCoeffs[AantalTaps] = 
{
512,512,512,512,512,512,
512,512,512,512,
/* ===centrum coefficient=== */
512,
/* ===centrum coefficient=== */

512,512,512,512,512,
512,512,512,512,