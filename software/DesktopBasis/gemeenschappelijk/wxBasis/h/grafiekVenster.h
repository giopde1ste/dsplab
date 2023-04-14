/*!
  @file 
  De grafische funkties API. Hier staat wxWidget code om grafieken in
  assenstelsels te tekenen.

  Dit bestand biedt een vereenvoudigde grafische interface voor gebruik bij het schrijven van een op
  wxWidgets gebaseerd programma voor het DSB practicum.
  De bovenste API is de oorspronkelijke Nederlandstalige API.

  This file offers a simplified graphical interface to be used when writing a wxWidgets based program
  in the Digital Signal Processing practial work assignments.
  Below the Dutch API, an English API has been constructed that offers the same functionality as the Dutch API.

  @version $Rev: 4707 $
  @author $Author: ewout $ 
  @copyright Copyright 2006-2019 ir drs E.J Boks Hogeschool van Arnhem en Nijmegen
  $Id: grafiekVenster.h 4707 2023-03-13 16:48:15Z ewout $
************************************************************************/

#ifndef HANESE_GRAFIEK_H
#define HANESE_GRAFIEK_H

// For compilers that support precompilation, includes "wx/wx.expAverage".
#include "wx/wxprec.h"

/* GUI includes */
#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/scrolwin.h>
#include <wx/colour.h>
#include <wx/brush.h>
#include <wx/dcclient.h>
#include <wx/sizer.h>
#include <wx/dcmemory.h>
#include <wx/dcbuffer.h>
#include <wx/font.h>
#include <wx/config.h>
#include <wx/string.h>
#include <wx/dynarray.h>

#include "algdef.h"


/* Dit is de Nederlandstalige API.
 * Below you can find the English language API */

/*! @brief Een PuntLijst is een wxArray van wxPoints.
 * een wxArray is een dynamische array, te vergelijken met een oneindig grote c array.
 * Zie http://docs.wxwidgets.org/3.0/classwx_array_3_01_t_01_4.html
 */
WX_DECLARE_OBJARRAY(wxPoint, PuntLijst);

class LabelPoint : public wxPoint
{
public:
	using wxPoint::wxPoint;

	LabelPoint(const wxPoint &, const wxString&);
	LabelPoint(const int , const int , const wxString &);

	/*! @brief Een optioneel label. Indien leeg (standaard) wordt niets geprint.
	 Als label niet leeg is, dan wordt de waarde rechts naast het punt geprint. */
	wxString label;
};

/*! @brief Een wxArray van Label wxPoints. */
WX_DECLARE_OBJARRAY(LabelPoint, LabelPuntLijst);

/*! @class Een lijnstuk bestaat uit twee wxPoints met een optioneel bolletje aan de uiteinden. */
class LijnStuk
{
public:
	explicit LijnStuk(const wxPoint &,
	                  const wxPoint &,
	                  const bool bolletje = false);

	wxPoint geefBegin() const;
	wxPoint geefEind() const;

	bool metBolletjes() const;

private:
	wxPoint begin, eind;
	bool bolleke;
};

/*! @brief Een LijnLijst is een wxArray van LijnStuk.
 */
WX_DECLARE_OBJARRAY(LijnStuk, LijnLijst);


/*! @brief deze klasse maakt het mogelijk op eenvoudige manier te plotten in een wxWidgets app*/
class GrafiekVenster : public wxPanel
{

public:

	explicit GrafiekVenster(wxWindow *parent,
	                        wxWindowID winid,
	                        const wxPoint& pos = wxDefaultPosition,
	                        const wxSize& size = wxDefaultSize,
	                        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
	                        const wxString& name = wxPanelNameStr)  : GrafiekVenster(parent,winid,size,style)
	{
		SetPosition(pos);
	};


	GrafiekVenster(wxWindow* parent,
	               int id,
	               const wxSize &size,
	               long style=wxTAB_TRAVERSAL);


	~GrafiekVenster() override;

	/*! @brief Zet de oorsprong absoluut */
	void zetOorsprong(const wxPoint &);

	/*! @brief Zet oorsprong relatief tot grafiekgrootte */
	void zetOorsprong(const wxRealPoint &);

	/*! @brief Zet de oorsprong midden, voor tekenen van grafiek met x en y as */
	void zetOorsprongMidden();
	/*! @brief Zet de oorsprong middenlinks, voor tekenen van grafiek met x en y as */
	void zetOorsprongLinksMidden();

	/*! @brief Geef de absolute koordinaten van de oorsprong.
	 * @return De koords.
	 */
	wxPoint geefOorsprong() const;

	/*! @brief zet een globale offsetfaktor */
	void zetOffset(const wxPoint &);

	/*! @brief geef de huidige offset. */
	wxPoint geefOffset() const;

	/*! @brief Geef de grootte van het tekenveld in pixels. @return De grootte als wxSize */
	wxSize geefTekenVeldGrootte() const;
	/*! @brief Maak het scherm schoon */
	void maakSchoon();

	enum class FontSoort
	{
		Micro,
		Mini,
		Klein,
		Normaal,
		Groot
	};

	/*! @brief plaats een string op de aangegeven lokatie in klein font.
	 * @param[in] De string die geplaatst wordt.
	 * @param[in] De lokatie. */
	void zetTekst(const FontSoort, 
				  const wxPoint &,
				  const wxString &);


	void zetTekst(const wxFont &,
		const wxPoint &,
		const wxString &);

	/*! @brief plaats een string op de aangegeven lokatie in klein font.
	 * @param[in] De string die geplaatst wordt.
	 * @param[in] De lokatie. */
	void zetKleineTekst(const wxString&,
		const wxPoint&);

	/*! @brief plaats een string op de aangegeven lokatie in normaal font.
	 * @param[in] De string die geplaatst wordt.
	 * @param[in] De lokatie. */
	void zetNormaleTekst(const wxString &,
	                     const wxPoint &);

	/*! @brief plaats een string op de aangegeven lokatie in groot font.
	 * @param[in] De string die geplaatst wordt.
	 * @param[in] De lokatie. */
	void zetGroteTekst(const wxString &,
	                   const wxPoint &);

	wxSize geefTekstGrootte(const FontSoort, const wxString&) const;

	void zetTekenPen(const wxPen &);  /*! @brief Zet pen om te tekenen */
	void zetAssenPen(const wxPen &);  /*! @brief Zet pen om assenstelsel te tekenen */

	void zetBorstel(const wxBrush &);  /*! @brief Zet de borstel om vlakken/cirkels e.d te vullen. */

	void zetStaafBreedte(const UInt32);
	void zetAutoStaafBreedte();
	
	void zetStdStaafBreedte();


	void tekenAssenstelsel();   /*! @brief Teken een assenstelsel */

	/*! @brief Teken een staaf van (coord_x,0) --> coord.
	 * @param[in] De coordinaat als wxPoint.
	 * @param[in] Geef aan of een bolleke (true) of niet (false) wordt getekend aan het eind van de staaf. */
	void tekenStaaf(const wxPoint &,
	                const bool bolletje=false);

	/*! @brief Teken een staaf van (coord_x,0) --> coord, en plaats de waarde van coord in mini font naast de staaf.
	 * @param[in] De coordinaat als wxPoint.
	`* @param[in] Geef aan of een bolleke (true) of niet (false) wordt getekend aan het eind van de staaf. */
	void tekenStaaf(const LabelPoint&,
					const bool bolletje = false);

	/*! @brief Teken een verzameling staven (in wxArray PuntLijst) van (coord_x,0) --> coord.
	 * @param[in] De verzameling wxPoints, verzameld in een PuntLijst. 
	 * @param[in] Geef aan of de grafiek autoschaling aanzet om alle staven binnen het veld te plaatsen.
	 * @param[in] Geef aan of een bolleke (true) of niet (false) wordt getekend aan het eind van elke staaf.
	 * @return de gebruikte schaalfaktoren in x en y richting (in een wxRealpoint).
	 * @note Bij autoschaling wordt de grootte bepaald door de grootte (x,y) coordinaten in de PuntLijst.
	         De staven worden automatisch geschaald naar 95% van het bereik. */
	wxRealPoint tekenStaven(const PuntLijst &,
							 const bool autoschaal,
	                        const bool marker=false);

	/*! @brief Teken een verzameling staven (in wxArray PuntLijst) van (coord_x,0) --> coord, en plaats de waarden van coord in mini font naast de staaf.
	 * @param[in] De verzameling wxPoints, verzameld in een PuntLijst.
	 * @param[in] Geef aan of de grafiek autoschaling aanzet om alle staven binnen het veld te plaatsen.
	 * @param[in] Geef aan of een bolleke (true) of niet (false) wordt getekend aan het eind van elke staaf.
	 * @return de gebruikte schaalfaktoren in x en y richting (in een wxRealpoint).
	 * @note Bij autoschaling wordt de grootte bepaald door de grootte (x,y) coordinaten in de PuntLijst.
			 De staven worden automatisch geschaald naar 95% van het bereik. */
	wxRealPoint tekenStaven(const LabelPuntLijst&,
							const bool autoschaal,
							const bool marker = false);

	/*! @brief Teken een verzameling staven (in wxArray PuntLijst) van (coord_x,0) --> coord.
	* @param[in] De verzameling wxPoints, verzameld in een PuntLijst.
	* @param[in] De gewenste schaling van ingegeven coordinaten naar schermcoordinaten.
	* @param[in] Geef aan of een bolleke (true) of niet (false) wordt getekend aan het eind van elke staaf. */
	void tekenStaven(const PuntLijst &,
	                 const wxRealPoint &,
	                 const bool marker=false);

	/*! @brief Teken een verzameling staven (in wxArray PuntLijst) van (coord_x,0) --> coord, en plaats de waarden van coord in mini font naast de staaf.
	* @param[in] De verzameling wxPoints, verzameld in een PuntLijst.
	* @param[in] De gewenste schaling van ingegeven coordinaten naar schermcoordinaten.
	* @param[in] Geef aan of een bolleke (true) of niet (false) wordt getekend aan het eind van elke staaf. */
	void tekenStaven(const LabelPuntLijst&,
					 const wxRealPoint&,
					 const bool marker = false);

	/*! @brief teken een lijn van coord1 naar coord2. 
	 * @param Het beginpunt van de lijn.
	 * @param Het eindpunt van de lijn.
	 * @param Moeten bolletjes de einpunten markeren? */
	void tekenLijn(const wxPoint &,
	               const wxPoint &,
	               const bool bolletje=false );

	/*! @brief teken een lijn van coord1 naar coord2 aan de hand van een LijnStuk klasse parameter.
	* @param begin en eind van de lijn .
	* @param Moeten bolletjes de einpunten markeren? */
	void tekenLijn(const LijnStuk &);

	/*! @brief teken een reeks van aaneengeschakelde lijnen.
	 * waarvan de coordinaten in de PuntLijst aanwezig zijn.
	 * @param  Een wxArray van punten.
	 * @param[in] Geef aan of de grafiek autoschaling aanzet om alle lijnen binnen het veld te plaatsen.
	 * @param Moeten bolletjes de einpunten markeren?
	 * @return de gebruikte schaalfaktoren in x en y richting (in een wxRealpoint). */
	wxRealPoint tekenLijnen(const LijnLijst &,
	                        const bool autoschaal = false);

	/*! @brief teken een reeks van aaneengeschakelde lijnen.
	* waarvan de coordinaten in de PuntLijst aanwezig zijn.
	* @param  Een wxArray van punten */
	void tekenLijnen(const LijnLijst &,
	                 const wxRealPoint &);

	/*! @brief teken een spline door een reeks van punten.
	 * De punten zijn in de PuntLijst aanwezig. 
	 * @param  Een wxArray van punten */
	void tekenSpline(const PuntLijst &);
	void tekenSpline(const LabelPuntLijst&);

	/*! @brief teken een rechthoek ergens in het tekenveld.
		@param De afmetingen van de rechthoek.
		@param moet de rechthoek gevuld worden? */
	void tekenRechthoek(const wxRect &, const bool);

	/*! @brief deze funkties tekenen lijnen die volledig doorlopen van begin tot eind van het tekenveld */
	void tekenVerticaleLijn(const wxCoord);      /* x coord */
	void tekenHorizontaleLijn(const wxCoord);    /* y coord */


	/*! @brief teken een cirkel.
	   @param het middelpunt,
	   @param de straal.
	   @ Moet de cirkel met de tekenpen gevuld worden ? */
	void tekenCirkel(const wxPoint &, const wxCoord, const bool );


	void tekenNulpunt(const wxPoint&, const wxCoord);
	void tekenPool(const wxPoint&, const wxCoord);


	/*! @brief exporteer het tekenveld als een wxImage */
	wxImage exporteerVeld() const;

	/*! @brief exporteer het tekenveld als een bestand (inclusief FileDialog) */
	void slaOpVeld(const wxString &titel=wxEmptyString) const;

	/*! @brief geef minima en maxima in de grafiek */
	wxCoord geefMinX() const;
	wxCoord geefMinY() const;
	wxCoord geefMaxX() const;
	wxCoord geefMaxY() const;

	/*! @brief converteer de muispositie naar de grafische coordinaat */
	wxPoint converteerMuisPositie(const wxMouseEvent &) const;
	
	wxSize grafiekGrootte;

private:
	
	static const wxFont microFont,miniFont,grootFont,normaalFont,kleinFont;
	static const wxBrush doorzichtig;
	static const wxBrush volledig;

	/* Attributen */
	wxPen tekenPen;
	wxPen assenstelselPen;
	wxPoint oorsprong,offset;
	
	UInt32 staafBreedte = 1;

protected:
	/* opslag van de grafiek */
	wxBitmap tekeningBitMap;
	wxMemoryDC tekeningDC;

	/* bewaar het vorige punt, indien nodig. */
	wxPoint vorigPunt;

	/*! @brief Converteer een grafiekcoord naar een schermcoord */
	wxPoint transCoord(const wxPoint &) const;
	/*! @brief Converteer een schermcoord naar een grafiekcoord */
	wxPoint transInverseCoord(const wxPoint &) const;

	/*! @brief Zet de tekenpen direkt zonder deze op te slaan in de variabele tekenpen. */
	void zetPen(const wxPen &);

	bool Layout(); /* overloaded uit basisklasse */
	virtual void teken(wxPaintEvent  &);
	void veegDCLeeg(wxEraseEvent &);


DECLARE_EVENT_TABLE();
};


class DynamischGrafiekVenster : public GrafiekVenster
{
public:
	
	using GrafiekVenster::GrafiekVenster;
	
	void initialiseer(const wxSize &nieuweGrootte);
	
	void teken(wxPaintEvent  &) override;
	
};
/* Below this line the English translation of all graphics functions can be found.
 * Please note that the Dutch versions are leading, ie they are needed at compilation time. */
#if (InterfaceTaalEnglish)
using PointList = PuntLijst;

/*! @class Een line piece is a line. It consists of two wxPoints with an optional round on the points. */
class LinePiece : public LijnStuk
{
public:
	explicit LinePiece(const wxPoint &a,
	                  const wxPoint &b,
	                  const bool round = false) : LijnStuk(a,b,round)
	{

	};

	wxPoint getBegin() const
	{
		return(geefBegin());
	};

	wxPoint getEnd() const
	{
		return(geefEind());
	};

	bool withRounds() const
	{
		return(metBolletjes());
	};

};

/*! @brief a LineList is a wxArray of LinePiece.
 * a wxArray is a dynamic array, comparable with a endless c-style array.
 * For more information please consult:
 * http://docs.wxwidgets.org/3.0/classwx_array_3_01_t_01_4.html
 */
using LineList = LijnLijst;

/*! @class This class is meant for easy access to the wxWidgets graphical drawing routines and facilitates easy drawing
 * of base shapes. */
class GraphicsWindow : public GrafiekVenster
{

public:
	/*! @brief Constructor */
	GraphicsWindow(wxWindow* parent,
	               int id,
	               const wxSize &size,
	               long style=wxTAB_TRAVERSAL) : GrafiekVenster(parent,id,size,style)
	{

	};

	~GraphicsWindow() = default;

	/*! @brief Set the originin an absolute way */
	void setOrigin(const wxPoint &point)
	{
		zetOorsprong(point);
	};

	/*! @brief Set origin relative to graph size */
	void setOrigin(const wxRealPoint &point)
	{
		zetOorsprong(point);
	};

	/*! @brief Set the origin center, for drawing graph with x and y axis*/
	void setOriginCentre()
	{
		zetOorsprongMidden();
	};

	/*! @brief put the origin in the middle left, for drawing a graph with x and y axis */
	void setOriginLeftCentre()
	{
		zetOorsprongLinksMidden();
	};

	/*! @brief set a global offset factor*/
	void setOffset(const wxPoint &point)
	{
		zetOffset(point);
	};

	/*! @brief give the current offset. */
	wxPoint getOffset() const
	{
		const auto of(geefOffset());
		return(of);
	};

	/*! @brief get the size of the drawing area in pixels.
	 * @return The size as wxSize */
	wxSize getCanvasSize() const
	{
		const auto cs(geefTekenVeldGrootte());
		return(cs);
	};

	/*! @brief Clean the screen */
	void clearCanvas()
	{
		maakSchoon();
	}

	/*! @brief place a string in the specified location in small font.
	* @param [in] The string that is placed.
	* @param [in] The location. */
	void putSmallText(const wxString &text,
	                   const wxPoint &point)
	{
		zetKleineTekst(text, point);
	};

	/*! @brief place a string in the specified location in normal font. 
	 * @param [in] The string that is placed. 
	 * @param [in] The location. */
	void putNormalText(const wxString &text,
	                   const wxPoint &point)
	{
		zetNormaleTekst(text, point);
	};

	/*!@brief place a string at the indicated location in large font.
	  * @param [in] The string that is placed.
	  * @param [in] The location. */
	void putLargeText(const wxString &text,
	                  const wxPoint &point)
	{
		zetGroteTekst(text, point);
	};

	/*! @brief Set pen for drawing. */
	void setDrawingPen(const wxPen &pen)
	{
		zetTekenPen(pen);
	};

	/*! @brief Set pen to draw coordinate system.  */
	void setAxesPen(const wxPen &pen)
	{
		zetAssenPen(pen);
	};

	/*! @brief Draw a coordinate system. */
	void drawAxisLines()
	{
		tekenAssenstelsel();
	};

	/*! @brief Draw a bar of (coord_x, 0) -> coord.
	  * @param [in] The coordinate as wxPoint.
	 * @param [in] get to whether a bolleke (true) or not (false) is drawn at the end of the bar. */
	void drawBar(const wxPoint &point,
	                const bool marker=false)
	{
		tekenStaaf(point,marker);
	};

	/*!@brief Draw a collection of bars (in wxArray PointList) from (coord_x, 0) -> coord.
* @param [in] The collection of wxPoints, collected in a Point List.
* @param [in] get to see if the auto scaling chart turns on to place all bars within the field.
* @param [in] get to whether a bolleke (true) or not (false) is drawn at the end of each bar.
* @return the used scale factors in x and y direction (in a wxRealpoint).
* @note With auto scaling, the size is determined by the size (x, y) coordinates in the Point List.
The bars are automatically scaled to 95% of the range. */
	wxRealPoint drawBars(const PointList &pl,
	                        const bool autoScale = false,
	                        const bool marker=false)
	{
		const auto rv(tekenStaven(pl,autoScale,marker));
		return(rv);
	};

	/*! @brief Draw a collection of bars (in wxArray PointList) from (coord_x, 0) -> coord.
* @param [in] The collection of wxPoints, collected in a Point List.
* @param [in] The desired scaling of entered coordinates to screen coordinates.
* @param [in] get to whether a bolleke (true) or not (false) is drawn at the end of each bar. */
	void drawBars(const PointList &pl,
	                 const wxRealPoint &rp,
	                 const bool marker=false)
	{
		tekenStaven(pl,rp,marker);
	}

	/*!@brief draw a line from coord1 to coord2.
* @param The starting point of the line.
* @param The end point of the line.
* @param Should balls mark the end points? */
	void drawLine(const wxPoint &beginPoint,
	               const wxPoint &endPoint,
	               const bool marker=false )
	{
		tekenLijn(beginPoint,endPoint,marker);
	};

	/*!@brief draw a line from coord1 to coord2 based on a LijnPiece class parameter.
     * @param start and end of the line.
     * @param Should balls mark the end points? */
	void drawLine(const LinePiece &lp)
	{
		tekenLijn(lp);
	}

	/*!@brief draw a series of concatenated lines.
	* whose coordinates are present in the PuntLijst.
	* @param A wxArray of points.
	* @param [in] get to see if the auto scaling chart turns on to place all lines within the field.
	* @param Should balls mark the end points?
	* @return the used scale factors in x and y direction (in a wxRealpoint). */
	wxRealPoint drawLines(const LineList &ll,
	                        const bool autoScale = false)
	{
		const auto rv(tekenLijnen(ll,autoScale));
		return(rv);
	};

	/*! @brief draw a series of concatenated lines.
	 * whose coordinates are present in the PuntLijst.
	* @param A wxArray of points */
	void drawLines(const LineList &ll,
	                 const wxRealPoint &rp)
	{
		tekenLijnen(ll,rp);
	};

	/*! @brief draw a spline through a series of points.
	 * The points are present in the Point List.
     * @param A wxArray of points */
	void drawSpline(const PointList &pl)
	{
		tekenSpline(pl);
	};

	/*! @brief draw a rectangle somewhere in the drawing area. */
	void drawRectangle(const wxRect &ra)
	{
		tekenRechthoek(ra);
	}

	/*! @brief these functions draw lines that run completely from start to finish of the drawing area.
	 * @param The x coordinate. */
	void drawVerticalLine(const wxCoord coord)
	{
		tekenVerticaleLijn(coord);
	};

	/*! @brief these functions draw lines that run completely from start to finish of the drawing area.
	 * @param The y coordinate. */
	void drawHorizontalLine(const wxCoord coord)
	{
		tekenHorizontaleLijn(coord);
	};

	/*! @brief Export the drawing area as a wxImage. */
	wxImage exportCanvas() const
	{
		const auto rv(exporteerVeld());
		return(rv);
	};

	/*! @brief export the drawing area as a file (including FileDialog) */
	void saveCanvas() const
	{
		slaOpVeld();
	};

	/*! @brief get minima and maxima in the graph */
	wxCoord getMinX() const
	{
		const auto rv(geefMinX());
		return(rv);
	};

	wxCoord getMinY() const
	{
		const auto rv(geefMinY());
		return(rv);
	};

	wxCoord getMaxX() const
	{
		const auto rv(geefMaxX());
		return(rv);
	};

	wxCoord getMaxY() const
	{
		const auto rv(geefMaxY());
		return(rv);
	};

	/*! @brief convert the mouse position to the graphical coordinate. */
	wxPoint convertMousePosition(const wxMouseEvent &event) const
	{
		const auto rv(converteerMuisPositie(event));
		return(rv);
	};

};


#endif

#endif   /* #ifndef */
