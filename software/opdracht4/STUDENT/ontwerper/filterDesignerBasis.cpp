
/*!
@file
Opdracht 4+6 DSB practicum (ontwerper gedeelte). 
Assignment 4+6 DSB practical (designer part). 
  LET OP : De student hoeft dit stuk NIET uit te werken 
  ATTENTION : no work is required from the student in this file.
@version $Rev: 298 $
@author $Author: ewout $

@copyright Copyright 2006-2020 ir drs E.J Boks, Hogeschool van Arnhem en Nijmegen. https://ese.han.nl

$URL: https://ese.han.nl/svn/dsbpracticum/trunk/2022/software/opdracht4/STUDENT/ontwerper/filterDesignerBasis.cpp $
$Id: filterDesignerBasis.cpp 298 2022-02-24 14:43:35Z ewout $
************************************************************************/

#include <cmath>

#include <filterDesignerBasis.h>

/*********** Geachte studenten , hieronder NIETS veranderen.              ******/
/*********** Honoured students, do NOT alter ANYTHING below this line.  ******/

unsigned int FilterDesignerBasis::FreqSpectrumPunten(const unsigned int aantalTaps)
{
	return(aantalTaps*16);
}

constexpr auto FilterDesignerBasis::MaximumBereikSignedInt(const unsigned int varbreedte)
{
	const auto bereik = (1<<((8*varbreedte)-1))-1;
	return(bereik);
}


double FilterDesignerBasis::compute_dB(const double lineair)
{
	const auto uit = 20.0*log10(fabs(lineair));
	return(uit);
}

double FilterDesignerBasis::compute_Linear(const double decibel)
{
	const auto uit = pow(10.0,decibel/20.0);
	return(uit);
}
