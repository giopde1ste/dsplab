/* Dit is een C/C++ Header bestand                                                                        */
/* Opdracht 4 Digitale Signaalbewerking/Digital Signal Processing  ESEDSP                                 */
/* Copyright 2006-2023 Opleiding Embedded Systems Engineering, Hogeschool van Arnhem en Nijmegen          */
/* Tijdstempel/Time Stamp : 2024-01-10 16:04:35 */
#ifndef FilterCoeffsExport_ESE_H
#define FilterCoeffsExport_ESE_H
/* Onderstaande coefficienten zijn voor een bandpass Q14 fixed-point implementatie FIR filter */
/* Start vd band : 0.050000*PI (200 Hz @ fs=4000 Hz) */
/* Eind vd band : 0.078750*PI (315 Hz @ fs=4000 Hz) */
/* Taps : 129 */



typedef signed short Int16;

static constexpr auto AantalTaps=129;
static constexpr auto SchaalFaktor=8192;

const Int16 filterFixedCoeffs[AantalTaps] = 
{
-4,-7,-8,-9,-7,-4,
1,8,16,22,26,
25,18,7,-7,-23,
-37,-46,-47,-40,-25,
-5,14,30,39,38,
30,17,5,0,4,
18,41,64,78,73,
44,-12,-89,-174,-248,
-291,-285,-219,-94,74,
261,432,550,585,518,
349,99,-193,-479,-707,
-828,-816,-664,-391,-43,
323,645,864,
/* ===centrum coefficient=== */
942,
/* ===centrum coefficient=== */
864,
645,323,-43,-391,-664,
-816,-828,-707,-479,-193,
99,349,518,585,550,
432,261,74,-94,-219,
-285,-291,-248,-174,-89,
-12,44,73,78,64,
41,18,4,0,5,
17,30,38,39,30,
14,-5,-25,-40,-47,
-46,-37,-23,-7,7,
18,25,26,22,16,
8,1,-4,-7,-9,
-8,-7,-4,
};

const float filterFloatCoeffs[AantalTaps] = 
{
-0.000244f,-0.000427f,-0.000488f,-0.000549f,-0.000427f,-0.000244f,
0.000061f,0.000488f,0.000977f,0.001343f,0.001587f,
0.001526f,0.001099f,0.000427f,-0.000427f,-0.001404f,
-0.002258f,-0.002808f,-0.002869f,-0.002441f,-0.001526f,
-0.000305f,0.000854f,0.001831f,0.002380f,0.002319f,
0.001831f,0.001038f,0.000305f,0.000000f,0.000244f,
0.001099f,0.002502f,0.003906f,0.004761f,0.004456f,
0.002686f,-0.000732f,-0.005432f,-0.010620f,-0.015137f,
-0.017761f,-0.017395f,-0.013367f,-0.005737f,0.004517f,
0.015930f,0.026367f,0.033569f,0.035706f,0.031616f,
0.021301f,0.006042f,-0.011780f,-0.029236f,-0.043152f,
-0.050537f,-0.049805f,-0.040527f,-0.023865f,-0.002625f,
0.019714f,0.039368f,0.052734f,
/* ===centrum coefficient=== */
0.057495f,
/* ===centrum coefficient=== */
0.052734f,
0.039368f,0.019714f,-0.002625f,-0.023865f,-0.040527f,
-0.049805f,-0.050537f,-0.043152f,-0.029236f,-0.011780f,
0.006042f,0.021301f,0.031616f,0.035706f,0.033569f,
0.026367f,0.015930f,0.004517f,-0.005737f,-0.013367f,
-0.017395f,-0.017761f,-0.015137f,-0.010620f,-0.005432f,
-0.000732f,0.002686f,0.004456f,0.004761f,0.003906f,
0.002502f,0.001099f,0.000244f,0.000000f,0.000305f,
0.001038f,0.001831f,0.002319f,0.002380f,0.001831f,
0.000854f,-0.000305f,-0.001526f,-0.002441f,-0.002869f,
-0.002808f,-0.002258f,-0.001404f,-0.000427f,0.000427f,
0.001099f,0.001526f,0.001587f,0.001343f,0.000977f,
0.000488f,0.000061f,-0.000244f,-0.000427f,-0.000549f,
-0.000488f,-0.000427f,-0.000244f,
};

#endif /* FilterCoeffsExport_ESE_H */
