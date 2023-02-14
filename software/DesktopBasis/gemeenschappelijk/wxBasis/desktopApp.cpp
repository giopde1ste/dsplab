/*!
@file
Desktop basisklasse implementatie voor HAN ESE applicaties.

@version $Rev: 4637 $
@author $Author: ewout $
@copyright Copyright 2006-2017 ir drs E.J Boks Hogeschool van Arnhem en Nijmegen
$Id: desktopApp.cpp 4637 2022-11-16 13:24:37Z ewout $
*/

#include <cstring>

#include <desktopApp.h>

#include <wx/utils.h>
#include <wx/filename.h>
#include <wx/filefn.h>
#include <wx/aboutdlg.h>
#include <wx/stdpaths.h>

#include <iostream>

ConsoleApp::ConsoleApp(const wxString &appNaam) : wxAppConsole()
{

	SetAppName(appNaam);

	const wxString bedrijfsNaam(wxT("Hogeschool van Arnhem en Nijmegen"));
	SetVendorName(bedrijfsNaam);
}


int ConsoleApp::OnExit()
{
	return(0);
}


DesktopApp::DesktopApp(const wxString &an) : appConfig(an,wxT("Hogeschool van Arnhem en Nijmegen")),
                                             enkeleInstantieControle(an+wxString::Format(wxT("-%s"),
                                                                                         wxGetUserId().c_str()),
                                                                     wxT("/tmp")),
                                             appNaam(an)
{
	if (true == enkeleInstantieControle.IsAnotherRunning())
		wxLogWarning(_("Please pay attention. The application is already running. Is this your intention?"));

	const wxFileName standaardConfigPad(wxGetHomeDir(),appNaam);
	const wxFileName standaardDataDirPad(wxGetHomeDir());

	const wxDateTime tijd = wxDateTime::Now();
	const wxString startdatum = tijd.FormatISODate();
	const wxString starttijd = tijd.FormatISOTime();

	/* Initialiseer de configuratie */
	appConfig.DontCreateOnDemand();

	//wxConfig::Set(&appConfig);   /* Zet dit als de default config */

	appConfig.Write(wxT("/Statistiek/StartDatum"),startdatum);
	appConfig.Write(wxT("/Statistiek/StartTijd"),starttijd);
	
	/* onthoud de werkdir en zet deze hier */
	wxString werkdir;
	appConfig.Read(wxT("/Operationeel/WerkDir"), &werkdir, wxFileName::GetHomeDir());
	wxSetWorkingDirectory(werkdir);
	
	int taalWaarde; // = wxLANGUAGE_DEFAULT;
	appConfig.Read(wxT("/Basisconfiguratie/Taal"),&taalWaarde,wxLANGUAGE_DEFAULT);

	taal =static_cast<wxLanguage>(taalWaarde);
	//taal =static_cast<wxLanguage>(wxLocale::GetSystemLanguage());
	
#ifdef __WXMSW__

	wxArrayString opzoekPaden;
	auto  stdPaden= wxStandardPaths::Get();
	const wxString resdir(stdPaden.GetResourcesDir());
	opzoekPaden.Add(resdir);

	

// add locale search paths
	const wxString exepad(stdPaden.GetExecutablePath());
	const wxFileName exedir(exepad);
	const wxString win32LokaalDir = exedir.GetPath(); // +wxT("\\share\\locale");
	wxLocale::AddCatalogLookupPathPrefix(win32LokaalDir);

	static const wxString winPaden[] = { wxT("\\share\\locale") , wxT("\\..\\share\\locale"),
										 wxT("\\..\\Debug\\share\\locale") ,wxT("\\..\\Release\\share\\locale") };

	for (auto& pad : winPaden)
	{
		const wxString nieuwPad(win32LokaalDir + pad);
		opzoekPaden.Add(nieuwPad);
	}

	for (auto& op : opzoekPaden)
	{
		wxLocale::AddCatalogLookupPathPrefix(op);
		wxLogDebug(wxT("Taalpad toegevoegd : ") + op);
	}

#endif


	if (false == lokaliteit.Init(taal,wxLOCALE_LOAD_DEFAULT))
	{
		const wxString taalString(lokaliteit.GetLanguageName(taal));
		wxLogDebug( wxT("Could not initialise the Locale for ") +
		            taalString+
		            wxT(", will opt for default English (NZ)"));
		taal=wxLANGUAGE_ENGLISH_NEW_ZEALAND;
		if (false == lokaliteit.Init(taal,wxLOCALE_LOAD_DEFAULT))
		{
			wxLogDebug( wxT("Could not initialise the Locale for ") +
			            taalString+
			            wxT("! No locale could be initialized."));
		}
	//	else
	//		wxLogDebug(_("Locale with English (NZ) initialized."));
	}
	else
	{
		wxLogDebug( _("The application was initialized to use the system default language, which has this name:")+
		            wxT(" \"")+lokaliteit.GetName()+wxT("\""));

		//const wxOperatingSystemId os = platform.GetOperatingSystemId();
		//wxArrayString opzoekPaden;

	/*	switch (os)
		{

			case wxOS_WINDOWS_9X:
			case wxOS_WINDOWS_NT:
			case wxOS_WINDOWS_MICRO:
			case wxOS_WINDOWS_CE:
			{
	
				static const wxString winPaden[] = { wxT("\\share\\locale") , wxT("\\..\\share\\locale"),
				                                     wxT("\\Debug\\share\\locale") ,wxT("\\Release\\share\\locale") };

				auto  stdPaden(wxStandardPaths::Get());


				for (auto& pad : winPaden)
				{
					const wxString nieuwPad(stdPaden.GetResourcesDir() + pad);
					opzoekPaden.Add(nieuwPad);
				}
				
			}
				break;

			default:
				break;
		}

		for (auto& op : opzoekPaden)
		{
			lokaliteit.AddCatalogLookupPathPrefix(op);
			wxLogDebug(wxT("Taalpad toegevoegd : ") + op);
		}
		*/
		
		lokaliteit.AddCatalog(appNaam);
		lokaliteit.AddCatalog(wxT("DSB"));
		lokaliteit.AddCatalog(wxT("RGT"));
	}

	wxLogDebug(wxT("Taal = ") + lokaliteit.GetCanonicalName());

	const wxString bedrijfsNaam(wxT("Hogeschool van Arnhem en Nijmegen"));

	SetAppName(appNaam);
	SetAppDisplayName(appNaam);

	SetVendorName(bedrijfsNaam);
	SetVendorDisplayName(bedrijfsNaam);

#if defined(__WXMSW__)
	/* 20.01.2022 : Zet hoge resolutie ook op Windows computers
	  Dit kan nog niet direkt met wxWidgets. Apple heeft eigen aanpak
	  in de info.plist.*/

	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
#endif	
}

DesktopApp::~DesktopApp()
{
	const wxDateTime tijd = wxDateTime::Now();
	const wxString stopdatum = tijd.FormatISODate();
	const wxString stoptijd = tijd.FormatISOTime();

	appConfig.Write(wxT("/Basisconfiguratie/Taal"),(int)taal);
	appConfig.Write(wxT("/Statistiek/StopDatum"),stopdatum);
	appConfig.Write(wxT("/Statistiek/StopTijd"),stoptijd);
	
	appConfig.Write(wxT("/Operationeel/WerkDir"), wxGetCwd());
	
	appConfig.Set(Null);

}



int DesktopApp::OnExit()
{
	return(0);
}

wxString DesktopApp::wxFindAppPath(const wxString& argv0,
                                   const wxString& cwd,
                                   const wxString& appVariableName)
{
	wxString str;

	// Try appVariableName
	if (!appVariableName.IsEmpty())
	{
		str = wxGetenv(appVariableName);
		if (!str.IsEmpty())
			return str;
	}

#if defined(__WXMAC__) && !defined(__DARWIN__)
	// On Mac, the current directory is the relevant one when
    // the application starts.
    return (cwd);
#endif

	if (wxIsAbsolutePath(argv0))
		return wxPathOnly(argv0);
	else
	{
		// Is it a relative path?
		wxString currentDir(cwd);
		if (currentDir.Last() != wxFILE_SEP_PATH)
			currentDir += wxFILE_SEP_PATH;

		str = currentDir + argv0;
		if (wxFileExists(str))
			return wxPathOnly(str);
	}

	// OK, it's neither an absolute path nor a relative path.
	// Search PATH.
	wxPathList pathList;
	pathList.AddEnvList(wxT("PATH"));
	str = pathList.FindAbsoluteValidPath(argv0);
	if (!str.IsEmpty())
		return(wxPathOnly(str));

	// Failed
	return(wxEmptyString);
}

wxString DesktopApp::geefOS() const
{
	const wxPlatformInfo info;

	return(info.GetOperatingSystemFamilyName());
}

wxOperatingSystemId DesktopApp::geefOSID() const
{
	const wxPlatformInfo info;

	return(info.GetOperatingSystemId());
}

wxString DesktopApp::geefOSNaam(const wxOperatingSystemId os) const
{
	wxString osNaam;

	switch(os)
	{

		case wxOS_MAC_OS:
			osNaam = wxT("Mac OS 8/9/X (Mac arch)");
			break;

		case wxOS_MAC_OSX_DARWIN:
			osNaam = wxT("Mac OS X (Darwin arch)");
			break;

		case wxOS_UNIX_LINUX:
			osNaam = wxT("Linux");
			break;
		case  wxOS_UNIX_FREEBSD:
			osNaam = wxT("FreeBSD");
			break;
		case wxOS_UNIX_OPENBSD:
			osNaam = wxT("OpenBSD");
			break;
		case wxOS_UNIX_NETBSD:
			osNaam = wxT("NetBSD");
			break;
		case wxOS_UNIX_SOLARIS:
			osNaam = wxT("SUN Solaris");
			break;
		case wxOS_UNIX_AIX:
			osNaam = wxT("IBM AIX");
			break;
		case wxOS_UNIX_HPUX:
			osNaam = wxT("HP-UX");
			break;

		case wxOS_WINDOWS_9X :
			osNaam = wxT("Microsoft Windows 95/98");
			break;
		case wxOS_WINDOWS_NT:
			osNaam = wxT("Microsoft Windows 2000/NT/XP");
			break;

		case wxOS_WINDOWS_MICRO:
			osNaam = wxT("Micro Windows");
			break;

		case wxOS_WINDOWS_CE:
			osNaam = wxT("Microsoft Windows CE");
			break;

		case wxOS_OS2:
			osNaam = wxT("IBM OS/2");
			break;

		default:
			osNaam = _("Unknown");
			break;
	}

	return(osNaam);
}

wxStandardPaths DesktopApp::geefStdPaden() const
{
	const auto uit =  wxStandardPaths::Get();
	return(uit);
}

wxString DesktopApp::geefAppNaam() const
{
	const wxString uit(appNaam);
	return(uit);
}

void DesktopApp::zetHoofdVenster(wxWindow * const win)
{
	wxASSERT_MSG((win != nullptr),wxT("(DesktopApp::zetHoofdVenster) win = NULL"));
	hoofdVenster = win;

	zetHoofdVensterGrootte();
	SetTopWindow(win);
}

wxConfig* DesktopApp::geefAppConfig() const
{
	auto uit = const_cast<wxConfig*>(&appConfig);
	return(uit);
}

void DesktopApp::zetTaal(const wxLanguage t)
{
	taal = t;
	wxASSERT_MSG((hoofdVenster != nullptr),wxT("(DesktopApp::zetTaal) win = NULL"));

	wxMessageBox(_("The application must now be restarted to activate the new language."),
	             _("Restart the application."), wxOK, hoofdVenster);
}

void DesktopApp::geefCopyright(const wxString &naam,
                               const wxString &beschrijving,
                               const wxString &copyright,
                               const wxString &versie)
{
	wxAboutDialogInfo info;

	const wxString platformString =  platform.GetPortIdName() + wxT(" ") +
	                                 platform.GetOperatingSystemIdName() +
	                                 wxT("(") + platform.GetBitnessName() + wxT(")");

#if (!(defined(__WXMAC__)))
	//const wxIcon ikoon((const char **)hanlogo);
    //info.SetIcon(ikoon);
#endif
	info.SetName(naam);
	info.SetVersion(_("Version : ")+versie);
	info.SetDescription(platformString + wxT("\n") + beschrijving);
	info.SetWebSite(wxT("https://ese.han.nl"));

	info.SetCopyright(copyright);

	wxArrayString ontwikkelaars;
	ontwikkelaars.Add(GebruikerNaam);
	info.SetDevelopers(ontwikkelaars);

	//	_T("(c)2006-2017 ir drs E.J Boks\nOpleiding Embedded Systems Engineering\nHogeschool van Arnhem en Nijmegen"));

	wxAboutBox(info);
}

void DesktopApp::geefCopyright(const wxString &naam,
                               const wxString &beschrijving,
                               const char **logo,
                               const wxString &copyright,
                               const wxString &versie)
{
	wxAboutDialogInfo info;
	const wxString platformString =  platform.GetPortIdName() + wxT(" ") +
	                                 platform.GetOperatingSystemIdName() +
	                                 wxT("(") + platform.GetBitnessName() + wxT(")");


#if (!(defined(__WXMAC__)))
	const wxIcon ikoon(logo);
    info.SetIcon(ikoon);
#endif
	info.SetName(naam);
	info.SetVersion(versie);
	info.SetDescription(platformString + wxT("\n") + beschrijving);
	info.SetWebSite("https://ese.han.nl");
	info.SetCopyright(copyright);
	//_T("(c)2006-2015 ir drs E.J Boks\nopleiding Embedded Systems Engineering\nHogeschool van Arnhem en Nijmegen"));

    wxArrayString ontwikkelaars;
    ontwikkelaars.Add(GebruikerNaam);
    info.SetDevelopers(ontwikkelaars);

	wxAboutBox(info);

}

void DesktopApp::zetHoofdVensterGrootte()
{
	const auto nuGrootte(hoofdVenster->GetScreenRect());

	appConfig.SetPath(wxT("/Gebruikersinstellingen/Venster"));

	wxPoint positie;
	wxSize grootte;
	appConfig.Read(wxT("KoordinaatX"), &(positie.x), nuGrootte.GetX());
	appConfig.Read(wxT("KoordinaatY"), &(positie.y), nuGrootte.GetY());
	appConfig.Read(wxT("GrootteX"), &(grootte.x), nuGrootte.GetWidth());
	appConfig.Read(wxT("GrootteY"), &(grootte.y), nuGrootte.GetHeight());

	const wxRect posGrootte(positie, grootte);
	hoofdVenster->SetSize(posGrootte);
	hoofdVenster->Layout();
}

void DesktopApp::bewaarHoofdVensterGrootte()
{
	const auto posGrootte(hoofdVenster->GetScreenRect());

	appConfig.SetPath(wxT("/Gebruikersinstellingen/Venster"));

	if ((posGrootte.GetWidth() > 100) && (posGrootte.GetHeight() > 100))
	{
		appConfig.Write(wxT("KoordinaatX"), posGrootte.GetX());
		appConfig.Write(wxT("KoordinaatY"), posGrootte.GetY());
		appConfig.Write(wxT("GrootteX"), posGrootte.GetWidth());
		appConfig.Write(wxT("GrootteY"), posGrootte.GetHeight());
	}
}

wxWindow *DesktopApp::geefHoofdVenster() const
{
	auto *ptr = hoofdVenster;
	wxASSERT(nullptr != ptr);
	return(ptr);
}


