/*!
  @file 
  De grafische funkties implementatie. Hier staat wxWidget code om grafieken in
  assenstelsels te tekenen.

  @version $Rev: 4707 $
  @author $Author: ewout $ 
  @copyright Copyright 2006-2015 ir drs E.J Boks Hogeschool van Arnhem en Nijmegen 
  $Id: grafiekVenster.cpp 4707 2023-03-13 16:48:15Z ewout $
*/

#include <grafiekVenster.h>

#include <wx/menu.h>

#include <wx/log.h>
#include <wx/pen.h>
#include <wx/dcclient.h>
#include <wx/msgdlg.h>
#include <wx/intl.h>
#include <wx/font.h>
#include <wx/fontenum.h>
#include <wx/fontmap.h>
#include <wx/filename.h>
#include <wx/filefn.h>
#include <wx/arrimpl.cpp> // This is a magic incantation which must be done!
#include <desktopApp.h>


/******************************* Multilijn 2D grafiek *********************/

WX_DEFINE_OBJARRAY(PuntLijst);
WX_DEFINE_OBJARRAY(LabelPuntLijst);
WX_DEFINE_OBJARRAY(LijnLijst);


LabelPoint::LabelPoint(const wxPoint &p, const wxString& l) : wxPoint(p), label(l)
{

}


LabelPoint::LabelPoint(const int xw, const int yw, const wxString& l) : wxPoint(xw, yw), label(l)
{

}


LijnStuk::LijnStuk(const wxPoint &b, const wxPoint &e,const bool mb) : begin(b), eind(e),bolleke(mb)
{

}

wxPoint LijnStuk::geefBegin() const
{
	return(begin);
}
wxPoint LijnStuk::geefEind() const
{
	return(eind);
}

bool LijnStuk::metBolletjes() const
{
	return(bolleke);
}

const wxFont GrafiekVenster::microFont = wxFont(6, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Tahoma"));
const wxFont GrafiekVenster::miniFont = wxFont(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Tahoma"));
const wxFont GrafiekVenster::kleinFont = wxFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Tahoma"));
const wxFont GrafiekVenster::normaalFont = wxFont(14, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Tahoma"));
const wxFont GrafiekVenster::grootFont = wxFont(60, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Tahoma"));

const wxBrush GrafiekVenster::doorzichtig = wxBrush(*wxWHITE, wxBRUSHSTYLE_TRANSPARENT);
const wxBrush  GrafiekVenster::volledig = wxBrush(*wxWHITE, wxBRUSHSTYLE_SOLID);


GrafiekVenster::GrafiekVenster(wxWindow* parent,
	int id,
	const wxSize& size,
	long style) : wxPanel(parent,
		id,
		wxDefaultPosition,
		size,
		(style | wxFULL_REPAINT_ON_RESIZE)),
	grafiekGrootte(GetSize()),
	tekenPen(wxPen(wxColour(wxT("BLUE")), 2, wxPENSTYLE_SOLID)),
	assenstelselPen(wxPen(wxColour(wxT("BLACK")), 1, wxPENSTYLE_DOT_DASH)),
	oorsprong(wxPoint(0, 0)),
	offset(oorsprong),
	tekeningBitMap(grafiekGrootte.GetWidth(),
		grafiekGrootte.GetHeight()),
	tekeningDC(tekeningBitMap)

{
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	SetAutoLayout(true);
	SetMaxSize(grafiekGrootte);

	wxASSERT_MSG((true == tekeningDC.IsOk()),
		wxT("(GrafiekVenster::GrafiekVenster) Kon geen geheugen voor DC alloceren"));

	if (true == tekeningDC.IsOk())
	{
		tekeningDC.Clear();
		tekeningDC.SetUserScale(1.0, 1.0);

		tekeningDC.SetAxisOrientation(true, false);
		tekeningDC.SetBrush(doorzichtig);
	}
}


GrafiekVenster::~GrafiekVenster()
{
	tekeningDC.SelectObject(wxNullBitmap);
}


BEGIN_EVENT_TABLE( GrafiekVenster, wxPanel)
				EVT_PAINT(GrafiekVenster::teken)
				EVT_ERASE_BACKGROUND(GrafiekVenster::veegDCLeeg)
END_EVENT_TABLE();




void GrafiekVenster::maakSchoon()
{
	tekeningDC.SetBrush(volledig);
	tekeningDC.Clear();
	
	Refresh();
}

wxSize GrafiekVenster::geefTekenVeldGrootte() const
{
	const auto gr = tekeningDC.GetSize(); // grafiekGrootte
	
	return(gr);
}

void GrafiekVenster::zetTekst(const FontSoort fs,
                              const wxPoint &punt,
                              const wxString &tekst)
{
	static const  wxFont *fpr[] = { &microFont, &miniFont, &kleinFont, &normaalFont, &grootFont };
	const auto index = static_cast<Teller>(fs);
	const auto f = *fpr[index];
	zetTekst(f, punt, tekst);
}

void GrafiekVenster::zetTekst(const wxFont &f,
                              const wxPoint& punt,
                              const wxString &tekst)
{
	/* zet het font */
	tekeningDC.SetFont(f);
	
	tekeningDC.DrawText(tekst, transCoord(punt));
}

void GrafiekVenster::zetKleineTekst(const wxString &tekst, const wxPoint &punt)
{
	zetTekst(FontSoort::Klein, punt, tekst);
}

void GrafiekVenster::zetNormaleTekst(const wxString& tekst, const wxPoint& punt)
{
	zetTekst(FontSoort::Normaal, punt, tekst);
}

void GrafiekVenster::zetGroteTekst(const wxString& tekst, const wxPoint& punt)
{
	zetTekst(FontSoort::Groot, punt, tekst);
}

wxSize GrafiekVenster::geefTekstGrootte(const GrafiekVenster::FontSoort fs, const wxString&tekst) const
{
	static const  wxFont* fpr[] = { &microFont, &miniFont, &kleinFont, &normaalFont, &grootFont };
	const auto index = static_cast<Teller>(fs);
	const auto f = fpr[index];
	int w, h;

	tekeningDC.GetTextExtent(tekst, &w, &h, nullptr, nullptr, f);
	const wxSize uit(w, h);
	return(uit);
}

void GrafiekVenster::zetOorsprong(const wxPoint &inOorsprong)
{
	
	wxASSERT_MSG( ((inOorsprong.x <= grafiekGrootte.GetWidth()) &&
	               (inOorsprong.y <= grafiekGrootte.GetHeight())),
	              wxT("(GrafiekVenster::zetOorsprong) oorsprong buiten veld."));
	
	oorsprong = inOorsprong;
}

void GrafiekVenster::zetOorsprong(const wxRealPoint &percOorsprong)
{
	wxASSERT(fabs(percOorsprong.x) <= 1.00);
	wxASSERT(fabs(percOorsprong.y) <= 1.00);
	
	const wxCoord xkoord(percOorsprong.x*grafiekGrootte.GetWidth());
	const wxCoord yKoord((percOorsprong.y*grafiekGrootte.GetHeight()));
	const wxPoint koord(xkoord,yKoord);
	
	zetOorsprong(koord);
}

void GrafiekVenster::zetOorsprongMidden()
{
	const wxPoint midden(grafiekGrootte.GetWidth()/2,grafiekGrootte.GetHeight()/2);
	
	zetOorsprong(midden);
}

void GrafiekVenster::zetOorsprongLinksMidden()
{
	const wxPoint midden(0, grafiekGrootte.GetHeight() / 2);
	
	zetOorsprong(midden);
}

wxPoint GrafiekVenster::geefOorsprong() const
{
	const auto uit(oorsprong);
	return(uit);
}

void GrafiekVenster::zetOffset(const wxPoint &of)
{
	offset = of;
}

/*! @brief geef de huidige offset. */
wxPoint GrafiekVenster::geefOffset() const
{
	return(offset);
}

wxPoint GrafiekVenster::transCoord(const wxPoint &inCoord) const
{
	const auto xkoord = oorsprong.x + tekeningDC.LogicalToDeviceX(inCoord.x);
	const auto ykoord = (geefMaxY()-(tekeningDC.LogicalToDeviceY(inCoord.y)));
	
	const wxPoint uitCoord(xkoord,
	                       ykoord);
	
	return(uitCoord);
}

wxPoint GrafiekVenster::transInverseCoord(const wxPoint &uitCoord) const
{
	const auto xkoord = tekeningDC.DeviceToLogicalX(uitCoord.x)-oorsprong.x;
	const auto ykoord = (geefMaxY()-(tekeningDC.DeviceToLogicalY(uitCoord.y)));
	
	const wxPoint inCoord(xkoord,ykoord);
	
	return(inCoord);
}

void GrafiekVenster::zetPen(const wxPen &pen)
{
	tekeningDC.SetPen(pen);
}

void GrafiekVenster::zetTekenPen(const wxPen &pen)
{
	tekenPen = pen;
}

void GrafiekVenster::zetAssenPen(const wxPen &pen)
{
	assenstelselPen = pen;
}

void GrafiekVenster::zetBorstel(const wxBrush &borstel)
{
	tekeningDC.SetBrush(borstel);
}

void GrafiekVenster::zetStaafBreedte(const UInt32 sb)
{
	staafBreedte = sb;
}

void GrafiekVenster::zetAutoStaafBreedte()
{
	staafBreedte = 0;
}

void GrafiekVenster::zetStdStaafBreedte()
{
	staafBreedte = 1;
}

void GrafiekVenster::tekenAssenstelsel()
{
	zetPen(assenstelselPen);
	
	const auto xasLinks(transCoord(wxPoint(geefMinX(),0)));
	const auto xasrechts(transCoord(wxPoint(geefMaxX(),0)));
	
	tekeningDC.DrawLine(xasLinks,
	                    xasrechts);
	
	const auto yasOnder(transCoord(wxPoint(0,geefMinY())));
	const auto yasBoven(transCoord(wxPoint(0, geefMaxY())));
	tekeningDC.DrawLine(yasOnder, yasBoven);
	
}

void GrafiekVenster::tekenStaaf(const wxPoint &coord,
								 const bool eindPunten)
{
	
	wxASSERT(0 != staafBreedte);

	zetPen(tekenPen);
	
	if (staafBreedte > 1)
	{
		const wxPoint basis(coord.x, 0);
		const wxSize gr(staafBreedte, coord.y);
		const wxRect rh(basis, gr);
		tekenRechthoek(rh,true);
		if (true == eindPunten)
			tekenCirkel(coord, staafBreedte,false);
	}
	else
	{
		const wxPoint basis(offset.x + coord.x, 0);
		const wxPoint eindPunt(transCoord((offset + coord)));
		tekeningDC.DrawLine(transCoord(basis), eindPunt);
		if (true == eindPunten)
			tekeningDC.DrawCircle(eindPunt, 1);
	}

	
	Refresh();
}

void GrafiekVenster::tekenStaaf(const LabelPoint& coord,
							     const bool eindPunten)
{
	
	const wxPoint koord(coord.x, coord.y);
	tekenStaaf(koord, eindPunten);
	
	if ((koord.y < vorigPunt.y-5) || (koord.y > vorigPunt.y + 5))
	{
		auto& label = coord.label;
		if (false == label.IsEmpty())
		{
			/* zet de waarde als tekst in het veld rechts naast de punt.*/
			zetTekst(GrafiekVenster::FontSoort::Mini, koord, label);
		}
		vorigPunt = koord;
	}
}

wxRealPoint GrafiekVenster::tekenStaven(const PuntLijst &punten,
                                        const bool autoschaal,
                                        const bool bolletje)
{
	wxRealPoint schaal;
	
	zetPen(tekenPen);
	
	if (false == autoschaal)
		schaal = wxRealPoint(1,1);
	else
	{
		wxPoint maxPunt(geefMinX(),geefMinY()),minPunt(geefMaxX(),geefMaxY());
		
		/* staven bevatten ook wxPoint(n,0) -> voeg de y koord=0 toe */
		if (0 > maxPunt.y)
			maxPunt.y = 0;
		
		if (0 < minPunt.y)
			minPunt.y = 0;
		
		/* schaal bepalen */
		const auto aantal = punten.GetCount();
		bool negxGebruikt = false, negyGebruikt = false;
		
		for (Teller i=0;i<aantal;i++)
		{
			const auto &punt  = punten[i];
			const auto xp = punt.x;
			const auto yp = punt.y;

//		wxLogDebug(wxT("punt = %d,%d"),punt.x,punt.y); 
			if (xp > maxPunt.x)
				maxPunt.x = xp;
			
			if (yp > maxPunt.y)
				maxPunt.y = yp;
			
			if (xp < minPunt.x)
				minPunt.x = xp;
			
			if (yp < minPunt.y)
				minPunt.y = yp;
			
			if (xp < 0)
				negxGebruikt = true;
			
			if (yp < 0)
				negyGebruikt = true;
		}
		
		
		double xSchaal;
		double ySchaal;
		
		if (false == negxGebruikt)
		{
			const double xBereik = geefMaxX();
			xSchaal = 0.95 * (xBereik) / (1.0 * (maxPunt.x - minPunt.x));
		}
		else
		{
			const double posxBereik = geefMaxX();
			const auto posx = 0.95 * (posxBereik) / (1.0 * (maxPunt.x - 0));
			
			const double negxBereik = geefMinX();
			const auto negx = 0.95 * (negxBereik) / (1.0 * (0 - minPunt.x));
			
			xSchaal = MAX(posx, negx);
		}
		
		if (false == negyGebruikt)
		{
			const double yBereik = geefMaxY();
			ySchaal = 0.95 * (yBereik) / (1.0 * (maxPunt.y - minPunt.y));
		}
		else
		{
			const double posyBereik = geefMaxY();
			const auto posy = 0.95 * (posyBereik) / (1.0 * (maxPunt.y - 0));
			
			const double negyBereik = geefMinY();
			const auto negy = 0.95 * (negyBereik) / (1.0 * (0 - minPunt.y));
			
			ySchaal = MAX(posy, negy);
		}
		
		schaal = wxRealPoint(xSchaal,ySchaal);
	}
	
	tekenStaven(punten, schaal, bolletje);
	
	return(schaal);
}

wxRealPoint GrafiekVenster::tekenStaven(const LabelPuntLijst& punten,
										const bool autoschaal,
                                        const bool bolletje)
{
	wxRealPoint schaal;
	
	zetPen(tekenPen);
	
	if (false == autoschaal)
		schaal = wxRealPoint(1, 1);
	else
	{
		wxPoint maxPunt(geefMinX(), geefMinY()), minPunt(geefMaxX(), geefMaxY());
		
		/* staven bevatten ook wxPoint(n,0) -> voeg de y koord=0 toe */
		if (0 > maxPunt.y)
			maxPunt.y = 0;
		
		if (0 < minPunt.y)
			minPunt.y = 0;
		
		/* schaal bepalen */
		const auto aantal = punten.GetCount();
		bool negxGebruikt = false, negyGebruikt = false;
		
		for (Teller i = 0; i < aantal; i++)
		{
			const auto& punt = punten[i];
			//		wxLogDebug(wxT("punt = %d,%d"),punt.x,punt.y); 
			const auto xp = punt.x;
			const auto yp = punt.y;
			
			//		wxLogDebug(wxT("punt = %d,%d"),punt.x,punt.y); 
			if (xp > maxPunt.x)
				maxPunt.x = xp;
			
			if (yp > maxPunt.y)
				maxPunt.y = yp;
			
			if (xp < minPunt.x)
				minPunt.x = xp;
			
			if (yp < minPunt.y)
				minPunt.y = yp;
			
			if (xp < 0)
				negxGebruikt = true;
			
			if (yp < 0)
				negyGebruikt = true;
		}
		
		double xSchaal;
		double ySchaal;
		
		if (false == negxGebruikt)
		{
			const double xBereik = geefMaxX();
			xSchaal = 0.95 * (xBereik) / (1.0 * (maxPunt.x - minPunt.x));
		}
		else
		{
			const double posxBereik = geefMaxX();
			const auto posx = 0.95 * (posxBereik) / (1.0 * (maxPunt.x - 0));
			
			const double negxBereik = geefMinX();
			const auto negx = 0.95 * (negxBereik) / (1.0 * (0 - minPunt.x));
			
			xSchaal = MAX(posx, negx);
		}
		
		if (false == negyGebruikt)
		{
			const double yBereik = geefMaxY();
			ySchaal = 0.95 * (yBereik) / (1.0 * (maxPunt.y - minPunt.y));
		}
		else
		{
			const double posyBereik = geefMaxY();
			const auto posy = 0.95 * (posyBereik) / (1.0 * (maxPunt.y - 0));
			
			const double negyBereik = geefMinY();
			const auto negy = 0.95 * (negyBereik) / (1.0 * (0 - minPunt.y));
			
			ySchaal = MAX(posy, negy);
		}
		
		schaal = wxRealPoint(xSchaal, ySchaal);
	}
	
	tekenStaven(punten, schaal, bolletje);
	
	return(schaal);
}

void GrafiekVenster::tekenStaven(const PuntLijst &punten,
                                 const wxRealPoint &schaal,
								 const bool bolletje)
{
	const auto aantal = punten.GetCount();

	/* breedte van de staaf hangt af van de schaal */
	const auto xBreedte = (0 != staafBreedte) ?  staafBreedte : MAX(1, static_cast<int>(schaal.x * 1));

	for (UInt32 i = 0; i < aantal; i++)
	{
		const auto& p = punten[i];
		const wxPoint basis(offset.x + static_cast<int>(schaal.x * p.x), offset.y);
		const wxPoint eind(basis.x + (xBreedte ), offset.y + static_cast<int>(schaal.y * p.y));
		const wxRect rh(basis, eind);

		tekenRechthoek(rh, true);
		//const wxPoint beginPunt(transCoord(basis));
		const wxPoint eindPunt(transCoord(eind));

		/* let op : size moet negatieve min waarde hebben om omhoog te tekenen. */
		//const wxSize rhGrootte(xBreedte, static_cast<int>(-1.0 * schaal.y * p.y));

		
		/* 12.03.2023 omgeschakeld naar staven omdat bij schaling dit beter werkt. */
		//tekeningDC.DrawRectangle(beginPunt, rhGrootte);
		//tekeningDC.DrawLine(beginPunt, eindPunt);
	//	const wxPoint vloedpunt(beginPunt.x + 1, beginPunt.y - 1);
	//	tekeningDC.FloodFill(vloedpunt, tekenPen.GetColour());

		if (true == bolletje)
		{
			static const UInt32 minBreedte = 2;
			const auto bolBreedte = MIN(minBreedte, xBreedte);
			tekeningDC.DrawCircle(eindPunt, bolBreedte);
		}
	}
	
	Refresh();
}

void GrafiekVenster::tekenStaven(const LabelPuntLijst& punten,
                                 const wxRealPoint& schaal,
								const bool bolletje)
{
	const auto aantal = punten.GetCount();

	/* breedte van de staaf hangt af van de schaal */
	const auto xBreedte = (0 != staafBreedte) ? staafBreedte : MAX(1, static_cast<int>(schaal.x * 1));

	for (UInt32 i = 0; i < aantal; i++)
	{
		const auto& p = punten[i];
		const wxPoint basis(offset.x + static_cast<int>(schaal.x * p.x), offset.y);
		const wxPoint eind(basis.x + (xBreedte / 2), offset.y + static_cast<int>(schaal.y * p.y));

		const wxPoint beginPunt(transCoord(basis));
		const wxPoint eindPunt(transCoord(eind));

		/* let op : size moet negatieve min waarde hebben om omhoog te tekenen. */
		const wxSize rhGrootte(xBreedte, static_cast<int>(-1.0 * schaal.y * p.y));

		/* 12.03.2023 omgeschakeld naar staven omdat bij schaling dit beter werkt. */
		tekeningDC.DrawRectangle(beginPunt, rhGrootte);
		

		//tekeningDC.DrawLine(beginPunt, eindPunt);

		if (true == bolletje)
		{
			static const UInt32 minBreedte = 2;
			const auto bolBreedte = MIN(minBreedte, xBreedte);
			tekeningDC.DrawCircle(eindPunt, bolBreedte);
		}
		
		if ((eind.y < vorigPunt.y - 5) || (eind.y > vorigPunt.y + 5))
		{
			const auto& label = p.label;
			if (false == label.IsEmpty())
			{
				/* zet de waarde als tekst in het veld rechts naast de punt.*/
				zetTekst(GrafiekVenster::FontSoort::Mini, eind, label);
			}
			
			vorigPunt = eind;
		}
	}
	
	Refresh();
}

void GrafiekVenster::tekenLijn(const wxPoint &begin,
                               const wxPoint &eind,
                               const bool eindPunten)
{
	const wxPoint beginPunt = transCoord(offset+begin),eindPunt = transCoord(offset+eind);
	
	zetPen(tekenPen);
	
	tekeningDC.DrawLine(beginPunt,eindPunt);
	
	if (eindPunten)
		tekeningDC.DrawCircle(eindPunt,2);
	
	Refresh();
}

void GrafiekVenster::tekenLijn(const LijnStuk &lijn)
{
	const auto begin = lijn.geefBegin();
	const auto einde = lijn.geefEind();
	const bool bolleke = lijn.metBolletjes();
	tekenLijn(begin, einde, bolleke);
}

wxRealPoint GrafiekVenster::tekenLijnen(const LijnLijst &lijnen,
                                        const bool autoschaal)
{
	wxRealPoint schaal;
	
	const auto aantal = lijnen.GetCount();
	
	if (false == autoschaal)
		schaal = wxRealPoint(1.0, 1.0);
	else
	{
		wxPoint maxPunt(geefMinX(), geefMinY()), minPunt(geefMaxX(), geefMaxY());
		
		
		/* staven bevatten ook wxPoint(n,0) -> voeg de y coord=0 toe */
		if (0 > maxPunt.y)
			maxPunt.y = 0;
		
		if (0 < minPunt.y)
			minPunt.y = 0;
		
		/* schaal bepalen */
		for (auto i = 0; i<aantal; i++)
		{
			const wxPoint punten[2] = { lijnen[i].geefBegin(),lijnen[i].geefEind() };
			
			for (auto &punt : punten)
			{
				//const wxPoint punt(punten[p]);
				//		wxLogDebug(wxT("punt = %d,%d"),punt.x,punt.y); 
				if ((punt.x) > maxPunt.x)
					maxPunt.x = punt.x;
				
				if ((punt.y) > maxPunt.y)
					maxPunt.y = punt.y;
				
				if ((punt.x) < minPunt.x)
					minPunt.x = punt.x;
				
				if ((punt.y) < minPunt.y)
					minPunt.y = punt.y;
			}
		}
		
		const double xBereik = geefMaxX() - geefMinX();
		const double yBereik = geefMaxY();
		
		const double xSchaal = 0.95*(xBereik) / (1.0*(maxPunt.x - minPunt.x));
		const double ySchaal = 0.95*(yBereik) / (1.0*(maxPunt.y - minPunt.y));
		
		schaal = wxRealPoint(xSchaal, ySchaal);
	}
	
	tekenLijnen(lijnen, schaal);
	
	return(schaal);
}

void GrafiekVenster::tekenLijnen(const LijnLijst &lijnen,
                                 const wxRealPoint &schaal)
{
	zetPen(tekenPen);
	const auto aantal = lijnen.GetCount();
	
	for (auto i = 0; i < aantal; i++)
	{
		const LijnStuk lijn(lijnen[i]);
		const wxPoint lijnBegin(lijn.geefBegin());
		const wxPoint lijnEind(lijn.geefEind());
		const wxPoint begin(offset.x + static_cast<int>(schaal.x*lijnBegin.x),
		                    offset.y + static_cast<int>(schaal.y*lijnBegin.y));
		const wxPoint eind(offset.x + static_cast<int>(schaal.x*lijnEind.x),
		                   offset.y + static_cast<int>(schaal.y*lijnEind.y));
		
		const wxPoint beginPunt(transCoord(begin));
		const wxPoint eindPunt(transCoord(eind));
		
		tekeningDC.DrawLine(beginPunt, eindPunt);
		
		if (true == lijn.metBolletjes())
		{
			tekeningDC.DrawCircle(beginPunt, 2);
			tekeningDC.DrawCircle(eindPunt, 2);
		}
	}
	Refresh();
}

void GrafiekVenster::tekenSpline(const PuntLijst &punten)
{
	const auto aantal = punten.GetCount();
	wxASSERT(0 != aantal);
	
	wxPoint* const nieuwePunten = new wxPoint[aantal ];
	for (auto i=0;i<aantal;i++)
		nieuwePunten[i] = transCoord(punten[i]+offset);
	
	zetPen(tekenPen);
	tekeningDC.DrawSpline(aantal, nieuwePunten);
	Refresh();
	
	delete[] nieuwePunten;
}

void GrafiekVenster::tekenSpline(const LabelPuntLijst& punten)
{
	const auto aantal = punten.GetCount();
	wxASSERT(0 != aantal);
	
	wxPoint* const nieuwePunten = new wxPoint[aantal];
	for (auto i = 0; i < aantal; i++)
		nieuwePunten[i] = transCoord(punten[i] + offset);
	
	zetPen(tekenPen);
	tekeningDC.DrawSpline(aantal, nieuwePunten);
	Refresh();
	
	delete[] nieuwePunten;
}

void GrafiekVenster::tekenRechthoek(const wxRect& rechthoek, const bool vul)
{
	auto rhGrootte = rechthoek.GetSize();
	const auto rhg = rhGrootte.GetHeight();
	if (rhg > 0)
		rhGrootte.SetHeight(rhg * -1);

	const wxRect rh(transCoord(offset + rechthoek.GetPosition()),
		rhGrootte);

	zetPen(tekenPen);
	if (true == vul)
	{
		const auto tekenBorstel(tekeningDC.GetBrush());
		const wxBrush vulBorstel(tekenPen.GetColour(),wxBRUSHSTYLE_SOLID);
	
		zetBorstel(vulBorstel);
		tekeningDC.DrawRectangle(rh);
		zetBorstel(tekenBorstel);
	}
	else
		tekeningDC.DrawRectangle(rh);

}

void GrafiekVenster::tekenVerticaleLijn(const wxCoord xCoord)
{
	const auto coord(transCoord(offset+wxPoint(xCoord,0)));
	
	zetPen(tekenPen);
	const wxPoint onderpunt(coord.x,0);
	const wxPoint bovenpunt(coord.x, grafiekGrootte.GetHeight());
	tekeningDC.DrawLine(bovenpunt,onderpunt);
	Refresh();
}

void GrafiekVenster::tekenHorizontaleLijn(const wxCoord yCoord)
{
	const auto coord(transCoord(offset+wxPoint(0,yCoord)));
	
	zetPen(tekenPen);
	const wxPoint linkspunt(0,coord.y);
	const wxPoint rechtspunt(grafiekGrootte.GetWidth(), coord.y);
	
	tekeningDC.DrawLine(linkspunt,rechtspunt);
	
	Refresh();
}


void GrafiekVenster::tekenCirkel(const wxPoint &mp, const wxCoord straal, const bool vul)
{
	tekeningDC.SetPen(tekenPen);
	const auto pm(transCoord(mp));

	if (true == vul)
	{
		const auto tekenBorstel(tekeningDC.GetBrush());
		const wxBrush vulBorstel(tekenPen.GetColour(), wxBRUSHSTYLE_SOLID);

		zetBorstel(vulBorstel);
		tekeningDC.DrawCircle(pm, straal);
		zetBorstel(tekenBorstel);
	}
	else
		tekeningDC.DrawCircle(pm, straal);
}


void GrafiekVenster::tekenNulpunt(const wxPoint &mp, const wxCoord breedte)
{
	tekenCirkel(mp, breedte/2, false);
}

void GrafiekVenster::tekenPool(const wxPoint& mp, const wxCoord breedte)
{
	const auto links = mp.x - breedte / 2;
	const auto rechts = mp.x + breedte / 2;
	const auto boven = mp.y + breedte / 2;
	const auto onder = mp.y - breedte / 2;
	
	const auto lb = wxPoint(links, boven);
	const auto lo = wxPoint(links, onder);
	const auto rb = wxPoint(rechts, boven);
	const auto ro = wxPoint(rechts, onder);
	
	
	const LijnStuk eerste(lb, ro);
	const LijnStuk tweede(rb, lo);
	
	tekenLijn(eerste);
	tekenLijn(tweede);
}

wxImage GrafiekVenster::exporteerVeld() const
{
	const auto beeld(tekeningBitMap.ConvertToImage());
	
	return(beeld);
}

void GrafiekVenster::slaOpVeld(const wxString &titel) const
{
	auto applikatie = reinterpret_cast<DesktopApp *>(wxTheApp);
	auto * const ouder = GetParent();
	
	const auto beeld(exporteerVeld());
	const wxString naam((true==titel.IsEmpty()) ? applikatie->geefAppNaam() : titel);
	const auto werkdir(wxGetCwd());
	
	wxFileName bestand(werkdir,naam);
	wxFileDialog dialoog(ouder,
	                     _("Save the image as a PNG file."),
	                     bestand.GetPath(),
	                     bestand.GetFullName(),
	                     wxT("Portable Network Graphics (*.png)|*.png"),
	                     wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
	
	if (wxID_OK == dialoog.ShowModal())
	{
		wxFileName pad(dialoog.GetPath());
		
		if (false == pad.HasExt())
			pad.SetExt(wxT("png"));
		
		const bool gelukt = beeld.SaveFile(pad.GetFullPath(),wxBITMAP_TYPE_PNG);
		
		if (true == gelukt)
			wxSetWorkingDirectory(pad.GetPath());
	}
}


wxCoord GrafiekVenster::geefMinX() const
{
	return(-1*oorsprong.x);
}

wxCoord GrafiekVenster::geefMinY() const
{
	return(-1*oorsprong.y);
}

wxCoord GrafiekVenster::geefMaxX() const
{
	return(grafiekGrootte.GetWidth()-oorsprong.x);
}

wxCoord GrafiekVenster::geefMaxY() const
{
	return(grafiekGrootte.GetHeight()-oorsprong.y);
}

wxPoint GrafiekVenster::converteerMuisPositie(const wxMouseEvent &event) const
{
	//const wxPoint coord(.x-oorsprong.x, event.GetPosition().y);
	const wxPoint convertResultaat(transInverseCoord(event.GetPosition()));//-oorsprong));
	return(convertResultaat);
}


bool GrafiekVenster::Layout()
{
	return(true);
}


void GrafiekVenster::teken(wxPaintEvent &event)
{
	wxAutoBufferedPaintDC bufferedDC(this);
	
	wxASSERT_MSG((bufferedDC.IsOk() == true),
	             wxT("(GrafiekVenster::GrafiekVenster) Kon geen geheugen voor Buffered DC alloceren"));
	
	bufferedDC.CopyAttributes(tekeningDC);
	
	if (false == bufferedDC.Blit(0,0,
	                             grafiekGrootte.GetWidth(),grafiekGrootte.GetHeight(),
	                             &tekeningDC,   /* bron */
	                             0,0))
	{
		wxLogError(wxT("(GrafiekVenster::teken) kon niet blit uitvoeren."));
	}
}


void GrafiekVenster::veegDCLeeg(wxEraseEvent &event)
{
	/* Hier doen we niets , ook geen skip, zodat er geen erase aktie is.*/
}


void DynamischGrafiekVenster::initialiseer(const wxSize &nieuweGrootte)
{
	tekeningDC.SelectObject(wxNullBitmap);
	const auto sk = 1.0; // GetDPIScaleFactor();
	tekeningBitMap.CreateWithDIPSize(grafiekGrootte, sk);
	tekeningDC.SelectObject(tekeningBitMap);
	
	wxASSERT_MSG((true == tekeningDC.IsOk()),
	             wxT("(GrafiekVenster::GrafiekVenster) Kon geen geheugen voor DC alloceren"));
	
	if (true == tekeningDC.IsOk())
	{
		grafiekGrootte = nieuweGrootte;
		tekeningDC.Clear();
		tekeningDC.SetUserScale(1.0, 1.0);
		
		tekeningDC.SetAxisOrientation(true, false);
	}
}


void DynamischGrafiekVenster::teken(wxPaintEvent &event)
{
	const auto ouderGrootte(wxWindow::GetClientSize());
	wxAutoBufferedPaintDC bufferedDC(this);
	
	wxASSERT_MSG((bufferedDC.IsOk() == true),
	             wxT("(GrafiekVenster::GrafiekVenster) Kon geen geheugen voor Buffered DC alloceren"));
	
	bufferedDC.CopyAttributes(tekeningDC);
	
	static const wxPoint startPunt;

	if (false == bufferedDC.StretchBlit(startPunt,ouderGrootte,
	                                    &tekeningDC,
	                                    startPunt,grafiekGrootte,wxCOPY,true))
	{
		wxLogError(wxT("(GrafiekVenster::teken) kon niet strechblit uitvoeren."));
	}
}


/*********** einde GrafiekVenster **************/
