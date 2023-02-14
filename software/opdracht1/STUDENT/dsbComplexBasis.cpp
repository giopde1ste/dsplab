/*!
@file

Opdracht 1 DSB practicum. Werk deze opdracht verder uit aan de hand van het kommentaar.
Assignment 1 DSB practical. Elaborate this assignment on the basis of the comments.

@version $Rev: 305 $
@author $Author: ewout $
@note  Werk deze code verder uit volgens de opdracht  / Complete this code according to the assignment.
@copyright Copyright 2006-2022 ir drs E.J Boks Hogeschool van Arnhem en Nijmegen.
$Id: dsbComplexBasis.cpp 305 2022-04-26 14:38:28Z ewout $
*/

#include <cmath>
#include <cassert>

#include <dsbComplex.h>

#ifdef BouwDesktopApp

#include <dsbComplexBasis.h>


PolairPrinter::PolairPrinter(const PolairGetal &pg) : p(pg)
{

};

wxString PolairPrinter::str(const PolairGetal &pg,const PolairPrinter::Formaat formaat, const Teller precisie)
{
	const wxString arg((Formaat::Standaard==formaat) ?
	                   wxString::Format(wxT("|%%.%df|*/_%%.%df"), precisie, precisie) :
	wxString::Format(wxT("%%.%df:%%.%df"), precisie, precisie));
	const wxString uit(wxString::Format(arg, pg.Mag(), pg.Arg()));
	return(uit);
}

wxString PolairPrinter::str(const Teller precisie) const
{
	const wxString uit(str(p,Formaat::Standaard,precisie));
	return(uit);
};

wxString PolairPrinter::wiskstr(const Teller precisie) const
{
	const wxString uit(str(p,Formaat::Importeerbaar,precisie));
	return(uit);
}

ComplexPrinter::ComplexPrinter(Complex &c) : z(c)
{

}


wxString ComplexPrinter::str(const Complex &cw,const ComplexPrinter::Formaat formaat)
{
	const wxString uit((Formaat::Standaard==formaat) ?
	                   wxString::Format(wxT("[Re:%.3f Im:%.3f]"), cw.Re(), cw.Im()) :
	                   wxString::Format(wxT("%.3f%+.3fj"), cw.Re(), cw.Im()));
	return(uit);
}

wxString ComplexPrinter::str() const
{
	const wxString uit(str(z,Formaat::Standaard));
	return(uit);
};

wxString ComplexPrinter::wiskstr() const
{
	const wxString uit(str(z,Formaat::Importeerbaar));
	return(uit);
}

RetCode ComplexPrinter::importeer(const wxString& invoer)
{
	auto retkode = RetCode::Onbekend;
	
	/* controleer of de string een imaginair stuk bevat */
	if (false == invoer.Contains(wxT("j")))
	{
		/* Kijk of het een polair getail is */
		if (false == invoer.Contains(wxT(":")))
		{
			/* Het is een volledig reel getal. */
			double waarde;
			const bool gelukt = invoer.ToDouble(&waarde);
			
			if (true == gelukt)
			{
				z.x = static_cast<float>(waarde);
				z.y = 0.0f;
			}
			retkode = (true == gelukt) ? RetCode::Ok : RetCode::Fout;
		}
		else
		{
			/* Het is een polair getal. */
			const wxString voorkomma(invoer.Before(':'));
			double rwaarde;
			const bool rgelukt = voorkomma.ToDouble(&rwaarde);
			if (false == rgelukt)
				retkode = RetCode::Fout;
			else
			{
				/* Doe phi */
				wxString nakomma(invoer.After(':'));
				
				/* kijk of het een hoek is :
				p ==> hoek is aangegeven in pi.
				d ==> hoek is aangegev en ion graden.
				k ==> hoek is aangegeven als 2*pi/k deel*/
				const bool pihoek = nakomma.StartsWith(wxT("p"));
				const bool gradhoek = nakomma.StartsWith(wxT("d"));
				
				/* mag niet tergelijkertijd, dan is er iets fout gegaan. */
				wxASSERT(false == ((true == pihoek) && (true == gradhoek)));
				
				if (true == pihoek)
					nakomma = nakomma.After('p');
				else if (true == gradhoek)
					nakomma = nakomma.After('d');
				
				double phiwaarde;
				
				const bool phigelukt = nakomma.ToDouble(&phiwaarde);
				
				if (true == phigelukt)
				{
					/* voer het getal in */
					if (true == pihoek)
						phiwaarde *= PI;
					else if (true == gradhoek)
						phiwaarde *= PI / 180.0f;
					
					const PolairGetal pg(static_cast<float>(rwaarde), static_cast<float>(phiwaarde));
					z = (pg);
				}
				
				retkode = ((true == phigelukt) ? RetCode::Ok : RetCode::Fout);
			}
		}
	}
	else
	{
		/* vind de reele en imaginaire komponenten. */
		wxString subinvoer(invoer.SubString(1, invoer.Length()));
		
		const bool posj = (true == subinvoer.Contains(wxT("+")));
		const bool negj = (true == subinvoer.Contains(wxT("-")));
		
		wxASSERT(posj != negj);
		
		if (true == posj)
		{
			const wxString voorj(invoer.Before('+'));
			double xwaarde;
			const bool xgelukt = voorj.ToDouble(&xwaarde);
			if (true == xgelukt)
				z.x = static_cast<float>(xwaarde);
			
			const wxString naplus(subinvoer.After('+'));
			const wxString imwaarde(naplus.Before('j'));
			double ywaarde;
			const bool ygelukt = imwaarde.ToDouble(&ywaarde);
			if (true == ygelukt)
				z.y = static_cast<float>(ywaarde);
			
			retkode = ((true == xgelukt) && (true == ygelukt)) ? RetCode::Ok : RetCode::Fout;
		}
		else
		{
			const wxString voorj(invoer.Before('-'));
			double xwaarde;
			const bool xgelukt = voorj.ToDouble(&xwaarde);
			if (true == xgelukt)
				z.x = static_cast<float>(xwaarde);
			
			const wxString naplus(subinvoer.After('-'));
			const wxString imwaarde(naplus.Before('j'));
			double ywaarde;
			const bool ygelukt = imwaarde.ToDouble(&ywaarde);
			if (true == ygelukt)
				z.y = -1.0f * static_cast<float>(ywaarde);
			
			retkode = ((true == xgelukt) && (true == ygelukt)) ? RetCode::Ok : RetCode::Fout;
		}
	}
	
	return(retkode);
}



#endif