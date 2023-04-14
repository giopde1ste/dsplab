/*!
@file
Desktop basisklasse API voor HAN ESE applikaties.

@version $Rev: 4693 $
@author $Author: ewout $
@copyright Copyright 2006-2019 ir drs E.J Boks Hogeschool van Arnhem en Nijmegen
$Id: desktopApp.h 4693 2023-03-09 15:32:45Z ewout $
*/

#ifndef DESKTOPAPP_H
#define DESKTOPAPP_H

#undef wxCRT_StrnlenA
#include <wx/wx.h>
#include <wx/app.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/filename.h>
#include <wx/config.h>
#include <wx/cmndata.h>
#include <wx/snglinst.h>

#include <algdef.h>
#include <desktopAppVersie.h>
#include <wx/stdpaths.h>

class ConsoleApp : public wxAppConsole
{
public:
    explicit ConsoleApp(const wxString &);    /* naam van de applicatie */
    ~ConsoleApp() = default;

    int OnExit();

    /* Taalinstellingen */
    wxLocale lokaliteit;
    wxLanguage taal;

protected:


};

class DesktopApp : public wxApp 
{
public:

    explicit DesktopApp(const wxString &);    /* naam van de applicatie */

    ~DesktopApp();

    int OnExit();

    void zetHoofdVenster(wxWindow * const);
	
	/* bewaar de venstergrootte */
    void zetHoofdVensterGrootte();
    void bewaarHoofdVensterGrootte();

	/* geef een pointer naar de config struct */
    wxConfig* geefAppConfig() const;

    void zetTaal(const wxLanguage );
	
    void geefCopyright(const wxString &,    /* naam */
                       const wxString &,    /* beschrijving */
					   const wxString &copyright=DesktopAppCopyright,
                       const wxString &versie=DesktopAppVersie);   

    void geefCopyright(const wxString &,     /* naam */
                       const wxString &,    /* beschrijving */
                       const char **,      /* XPM logo */
					   const wxString &copyright=DesktopAppCopyright,
                       const wxString &versie=DesktopAppVersie);

    wxString geefOS() const;
    wxOperatingSystemId geefOSID() const;

	wxStandardPaths geefStdPaden() const;

    wxString geefAppNaam() const;
	wxWindow *geefHoofdVenster() const;
	
	/*! @brief Zet " " om een string.
	* @param de string.
	* @param moeten er ook spaties om de string heen worden geplaatst, binnen de quotes?
	* @note Het " teken zelf word in de regel vervangen door een \" omdat de quotes anders plotseling eindigen.
	* @return de uitvoerstring met "string". */
	static wxString dubbeleQuotes(const wxString &,const bool metSpatie=false);
	
	/*! @brief Snelle kopie van bovenstaande funktie. */
	static wxString dq(const wxString &str,const bool metSpatie=false)
	{
		const wxString uit(dubbeleQuotes(str,metSpatie));
		return(uit);
	};
	
protected:

	RetCode initLokaliteit();

	/* Taalinstellingen */
	bool taalGekozen=false;
	/* Begin met een nullptr, in zetLokaliteit wordt deze geallokeerd. */
	wxTranslations *vertaler = nullptr;
	wxLocale *lokaliteit = nullptr;
	
	wxLanguage taal = wxLanguage::wxLANGUAGE_UNKNOWN;
	RetCode taalGoedGezet = RetCode::Onbekend;
	
    wxWindow *hoofdVenster = nullptr;
    
	/*! @brief geef het pad naar deze applikatie. */
	wxFileName geefExePad() const;
	
	wxString geefOSNaam(const wxOperatingSystemId ) const;   /* id --> naam in wxString */
 
private:

    wxConfig appConfig;
	wxSingleInstanceChecker enkeleInstantieControle;
    const wxString applikatieNaam;
    
    const wxPlatformInfo platform;
	wxStandardPaths stdPaden;
	
	RetCode initLokaliteit(wxLanguage nieuweTaal);
	
	RetCode zetLokaliteit(const wxLanguage nieuweTaal, const bool slaMeteenOp);
	
	RetCode geefLokaliteitStatus() const;
};

extern char const * const hanlogo[];

#endif
