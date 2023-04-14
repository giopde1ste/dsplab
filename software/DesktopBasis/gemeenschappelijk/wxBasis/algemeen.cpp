/*!
@file
algemene software voor HAN ESE applicaties.

@version $Rev: 4685 $
@author $Author: ewout $
@copyright Copyright 2006-2023 ir drs E.J Boks Hogeschool van Arnhem en Nijmegen
$Id: algemeen.cpp 4685 2023-03-08 15:50:32Z ewout $
*/

#include <desktopApp.h>
#include <wx/string.h>
#include <algemeen.h>
#include <tinyexpr.h>

double WiskundeUitdrukkingParser::evalueer(const wxString &uitdrukking)
{
	wxString gekuisd(uitdrukking);
	gekuisd.Replace(wxT(","),wxT("."));
	
	const auto expressie(gekuisd.c_str());
	const auto uit = te_interp(expressie, &fout);
	
	laatsteGoed = (0 == fout);
	
	if (false == laatsteGoed)
		wxLogError(_("The expression") +
		           DesktopApp::dq(uitdrukking, true) +
		           _("could not be computed into a numerical value."));
	else
		resultaat = uit;
	
	return (uit);
}

bool WiskundeUitdrukkingParser::goed() const
{
	return(laatsteGoed);
}

double WiskundeUitdrukkingParser::laatsteWaarde() const
{
	return(resultaat);
}
