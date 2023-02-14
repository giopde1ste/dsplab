/*!
@file
Opdracht 4 DSB practicum (applikatie API gedeelte).
 De student hoeft dit stuk NIET uit te werken / NO work is required from the student in this file/
@version $Rev: 298 $
@author $Author: ewout $

@copyright Copyright 2006-2020 ir drs E.J Boks, Hogeschool van Arnhem en Nijmegen. https://ese.han.nl

$URL: https://ese.han.nl/svn/dsbpracticum/trunk/2022/software/opdracht4/Basis/h/applikatie4.h $
$Id: applikatie4.h 298 2022-02-24 14:43:35Z ewout $
************************************************************************/

#ifndef OPDRACHT4_MAIN_H
#define OPDRACHT4_MAIN_H

#include <desktopApp.h>

class FilterVenster;

/************* Applicatie ( de "main()" van deze GUI applikatie ******/
class FirFilterKlasse : public DesktopApp
{

public:

    FirFilterKlasse();

    bool OnInit();
    int OnExit();

private:

	FilterVenster *filterVenster= nullptr;

};

DECLARE_APP(FirFilterKlasse);

#endif
