/*!
@file
Desktop basisklasse API voor HAN ESE applikaties.

@version $Rev: 4637 $
@author $Author: ewout $
@copyright Copyright 2006-2019 ir drs E.J Boks Hogeschool van Arnhem en Nijmegen
$Id: desktopApp.h 4637 2022-11-16 13:24:37Z ewout $
*/

#ifndef DESKTOPAPP_H
#define DESKTOPAPP_H

#undef wxCRT_StrnlenA
#include <wx/wx.h>
#include <wx/app.h>
#include <wx/image.h>
#include <wx/intl.h>
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

protected:

    /* Taalinstellingen */
    wxLocale lokaliteit;
    wxLanguage taal;
	
    wxWindow *hoofdVenster = nullptr;
	
    wxString wxFindAppPath(const wxString& argv0, 
                           const wxString& cwd, 
                           const wxString& appVariableName);
    

    wxString geefOSNaam(const wxOperatingSystemId ) const;   /* id --> naam in wxString */


	
private:

    wxConfig appConfig;
	wxSingleInstanceChecker enkeleInstantieControle;
    const wxString appNaam;
    const wxPlatformInfo platform;

};

extern char const * const hanlogo[];

#endif
