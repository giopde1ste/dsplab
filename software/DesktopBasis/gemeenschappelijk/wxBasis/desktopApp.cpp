/*!
@file
Desktop basisklasse implementatie voor HAN ESE applicaties.

@version $Rev: 4697 $
@author $Author: ewout $
@copyright Copyright 2006-2023 ir drs E.J Boks Hogeschool van Arnhem en Nijmegen
$Id: desktopApp.cpp 4697 2023-03-10 16:12:02Z ewout $
*/

#include <cstring>

#include <desktopApp.h>

#include <wx/utils.h>
#include <wx/filename.h>
#include <wx/filefn.h>
#include <wx/aboutdlg.h>
#include <wx/stdpaths.h>
#include <wx/uilocale.h>

#include <iostream>

ConsoleApp::ConsoleApp(const wxString &appNaam) : wxAppConsole()
{
	/* Gebruik de lokaal instellingen van deze computer. */
	wxUILocale::UseDefault();
	
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
                                             applikatieNaam(an),
                                             stdPaden(wxStandardPaths::Get())
{
	if (true == enkeleInstantieControle.IsAnotherRunning())
		wxLogWarning(_("Please pay attention. The application is already running. Is this your intention?"));

	const wxFileName standaardConfigPad(wxGetHomeDir(), applikatieNaam);
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
	
	
	static const wxString bedrijfsNaam(wxT("Hogeschool van Arnhem en Nijmegen"));

	SetAppName(applikatieNaam);
	SetAppDisplayName(applikatieNaam);

	SetVendorName(bedrijfsNaam);
	SetVendorDisplayName(bedrijfsNaam);

#if (!defined(__WXGTK_))
	initLokaliteit();
#endif

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
	
	if (nullptr != lokaliteit)
		delete lokaliteit;
}

wxString DesktopApp::dubbeleQuotes(const wxString &woord,const bool metSpatie)
{
	const wxString spatie((true==metSpatie) ? wxT(" ") : wxEmptyString);
	const auto uit(spatie+wxT("\"")+woord+wxT("\"")+spatie);
	return(uit);
}

RetCode DesktopApp::initLokaliteit()
{
	int taalvar = wxLocale::GetSystemLanguage();
	appConfig.SetPath(wxT("/Gebruikersinstellingen"));
	//	appConfig.Read(wxT("Encoding"), &taalvar, wxLocale::GetSystemLanguage());


	const auto eigenTaal = static_cast<wxLanguage>(taalvar);
	static const auto SysteemTaal = static_cast<wxLanguage>(wxLocale::GetSystemLanguage());

	/* Gebruik de lokaal instellingen van deze computer. */
	wxUILocale::UseDefault();

	/* pogingen : probeer de ingelezen taal, dan de systeemtaal en als laatste Engels */
	const wxLanguage taalPogingen[] = { eigenTaal,
										SysteemTaal,
										wxLANGUAGE_ENGLISH };

	static const wxString meldingen[] = { wxT("The application locale could not be set according to your preferences! A reset to the default system language will be attempted."),
										  wxT("The application locale could not be set according to the system language! The application will be reset to using the English language."),
										  wxT("The application locale could not be set according to the English language! Is your computer configured properly?") };

	auto taalGoed = RetCode::Onbekend;

	auto meldingIndex = 0;
	for (auto t : taalPogingen)
	{
		taalGoed = initLokaliteit(t);

		if (RetCode::Ok == taalGoed)
		{
			taal = t;
			break;
		}
		else
		{
			auto melding(meldingen[meldingIndex++]);
			wxLogDebug(melding);
			wxLogError(melding);
			/* Er is iets foutgegaan. Zet terug naar de standaardtaal. */
//			if (nullptr != lokaliteit)
//			{
//				delete lokaliteit;
//				lokaliteit = nullptr;
//			}
		}
	}

	return(taalGoed);
}

RetCode DesktopApp::initLokaliteit(wxLanguage nieuweTaal)
{
	auto retkode = RetCode::Ok;
	
	const wxString appNaam(geefAppNaam());
	
	// voor test: nieuweTaal = wxLANGUAGE_HEBREW;
	
	/* maak de taalkodes aangepast aan de standaarden die Kiwanda hanteert. */
	switch(nieuweTaal)
	{
		case wxLANGUAGE_CHINESE:
			nieuweTaal = wxLANGUAGE_CHINESE_TAIWAN;
			break;
		
		case wxLANGUAGE_HEBREW:
			nieuweTaal = wxLANGUAGE_HEBREW_ISRAEL;
			break;
		
		case wxLANGUAGE_JAPANESE:
			nieuweTaal = wxLANGUAGE_JAPANESE_JAPAN;
			break;
		
		case wxLANGUAGE_KOREAN:
			nieuweTaal = wxLANGUAGE_KOREAN_KOREA;
			break;
		
		case wxLANGUAGE_SWEDISH:
			nieuweTaal = wxLANGUAGE_SWEDISH_SWEDEN;
			break;
		
		default:
			/* laat de rest ongemoeid. */
			break;
	}

//	 wxLANGUAGE_JAPANESE_JAPAN)) //
	if (RetCode::Ok != zetLokaliteit(nieuweTaal,false))
	{
		wxLogError(wxT("Could not initialise the Internationalization Locale system for ") +
		           dq(appNaam) + wxT("."));
		taalGoedGezet = retkode = RetCode::Fout;
	}
	else
	{
		
		const wxOperatingSystemId os = platform.GetOperatingSystemId();
		wxArrayString opzoekPaden;
		
		const wxString resdir(stdPaden.GetResourcesDir());
		opzoekPaden.Add(resdir);
		
		switch (os)
		{
			
			case wxOS_WINDOWS_9X:
			case wxOS_WINDOWS_NT:
			case wxOS_WINDOWS_MICRO:
			case wxOS_WINDOWS_CE:
			{
				// add locale search paths
				const auto uitvoerpad(geefExePad());
				
				const wxString win32LokaalDir = uitvoerpad.GetPath(); // +wxT("\\share\\locale");
				wxFileTranslationsLoader::AddCatalogLookupPathPrefix(win32LokaalDir);

				wxLogDebug(wxT("Windows exe pad:") + uitvoerpad.GetFullPath());

#ifndef NDEBUG
				static const wxString winPaden[] = {wxT("\\share\\locale"),
				                                    wxT("\\..\\share\\locale")};
#else
				static const wxString winPaden[] = { wxT("\\share\\locale") ,
														wxT("\\..\\share\\locale") };
#endif
				
				
				for (auto &pad : winPaden)
				{
					const wxString nieuwPad(win32LokaalDir + pad);
					opzoekPaden.Add(nieuwPad);
				}
			}
				break;
			
			case wxOS_UNIX_FREEBSD:
			case wxOS_UNIX_OPENBSD:
			case wxOS_UNIX_NETBSD:
			case wxOS_UNIX_SOLARIS:
			case wxOS_UNIX_AIX:
			case wxOS_UNIX_HPUX:
			case wxOS_UNIX_LINUX:
			{
				const wxString taalAfkorting(wxLocale::GetLanguageCanonicalName(nieuweTaal));
				const wxString stdResources(stdPaden.GetResourcesDir());
				const auto uitvoerpad(geefExePad());
				const wxString uitvoerdir(uitvoerpad.GetPath());
				
				wxLogDebug(wxT("Taalpad (BSD/Solaris/AIX/Linux) toegevoegd."));
				wxLogDebug(wxT("WerkDir = ")+wxFileName::GetCwd());
                wxLogDebug(wxT("Uitvoeringspad = ")+uitvoerdir);

				opzoekPaden.Add(stdResources + wxT("/Kiwanda/") + applikatieNaam + wxT("/locale"));
				opzoekPaden.Add(stdResources + wxT("/../Kiwanda/") + applikatieNaam + wxT("/locale"));
				
				opzoekPaden.Add(stdResources + wxT("/locale"));
				opzoekPaden.Add(uitvoerdir + wxT("/share/Kiwanda/") + applikatieNaam + wxT("/locale"));
				opzoekPaden.Add(uitvoerdir + wxT("/../share/Kiwanda/") + applikatieNaam + wxT("/locale"));
			}
				break;
			
			default:
				/* op Apple is alles automatisch al goed */
				break;
			
		}
		
		for (auto &op : opzoekPaden)
		{
			//wxLocale::AddCatalogLookupPathPrefix(op);
			wxFileTranslationsLoader::AddCatalogLookupPathPrefix(op);
			wxLogDebug(wxT("Taalpad toegevoegd : ") + op);
		}
		
		if (nieuweTaal != wxLANGUAGE_ENGLISH)
		{
			const wxString taalString(wxLocale::GetLanguageName(nieuweTaal));
			
			/* voeg de std bieb toe. */
			if (false == vertaler->AddStdCatalog())
			{
				
				const wxString foutmelding(
						wxT("Could not add the standard wxWidgets catalog for language ") + DesktopApp::dq(taalString));

#ifdef NDEBUG
				wxLogError(foutmelding);
#else
				wxLogDebug(foutmelding);
#endif
				taalGoedGezet = retkode = RetCode::Fout;
			}
			
			const wxString katalogusNaam(wxT("DSB"));
			
			//KiwandaAssert(nullptr != lokaliteit);
//			if (false == lokaliteit->AddCatalog(katalogusNaam))
//			KiwandaAssert(nullptr != vertaler);
			if (false == vertaler->AddCatalog(katalogusNaam))
			{
				
				const wxString foutmelding(wxT("Could not add DSB catalog for language ") + DesktopApp::dq(taalString));
#ifdef NDEBUG
				wxLogError(foutmelding);
#else
				wxLogDebug(foutmelding);
#endif
				taalGoedGezet = retkode = RetCode::Fout;
			}
			else
			{
				wxLogDebug(wxT("DSB katalogus voor taal : ") +
				           taalString + wxT(" met succes toegevoegd."));
				taalGoedGezet = retkode = RetCode::Ok;
			}
		}
	}

#ifndef NDEBUG
	if (RetCode::Ok == retkode)
	{
		const wxString taalString(wxLocale::GetLanguageName(nieuweTaal));
		wxLogDebug(_("The internationalization locale for") +
		           dubbeleQuotes(taalString,true) +
		           _("was initialized successfully."));
	}
#endif
	
	return(retkode);
}

RetCode DesktopApp::zetLokaliteit(const wxLanguage nieuweTaal, const bool slaMeteenOp)
{
	auto retkode = RetCode::Onbekend;
	
	/* Zie:
	 * https://docs.wxwidgets.org/3.2/classwx_locale.html#a37c254f20d4862b6efea2fedf63a231a
	 */
	//nieuweTaal = wxLANGUAGE_CHINESE;
#ifndef __WXMAC__
	
	/* Schakel uit en begin opnieuw. */
	if(nullptr != lokaliteit)
	{
		delete lokaliteit;
		lokaliteit = nullptr;
	}

	lokaliteit = new wxLocale(nieuweTaal, wxLOCALE_LOAD_DEFAULT);
	const bool taalInit = lokaliteit->IsOk(); //.Init(nieuweTaal, wxLOCALE_LOAD_DEFAULT);
#else
	/* This class is known to have several problems under macOS:
	 * first of all, it is impossible to change the application UI locale after launching it under this platform
	 * and so using this class doesn't affect the native controls and dialogs there.
	 * Additionally, versions of macOS between 11.0 and 12.2 inclusive are affected by a bug
	 * when changing C locale can break display of the application menus.
	 * Because of this, it is recommended to use wxUILocale instead of this class
	 * for the applications targeting macOS. */
	static const bool taalInit=true;
#endif
	if (false == taalInit)
	{
		const wxString taalString(wxLocale::GetLanguageName(nieuweTaal));
		wxLogError(_("Could not initialise the Locale for") + DesktopApp::dq(taalString,true) +wxT("."));
		delete lokaliteit;
		lokaliteit = nullptr;
		retkode = RetCode::Fout;
	}
	else
	{
		if (nullptr != vertaler)
		{
			const wxString oudeTaalString(wxLocale::GetLanguageName(taal));
			const wxString oudeTaalNaam(wxLocale::GetLanguageCanonicalName(taal));
			
			wxLogDebug(wxT("(DesktopAppBasis::zetLokaliteit) oude taalkode = %d"), static_cast<UInt32>(taal));
			wxLogDebug(wxT("(DesktopAppBasis::zetLokaliteit) oude taalnaam = ") + oudeTaalString);
			wxLogDebug(wxT("(DesktopAppBasis::zetLokaliteit) oude taalkort = ") + oudeTaalNaam);
			/* wxTranslations vernietigt ons objekt want heeft ownership. */
		}
		
		const wxString taalString(wxLocale::GetLanguageName(nieuweTaal));
		const wxString taalNaam(wxLocale::GetLanguageCanonicalName(nieuweTaal));
		
		wxLogDebug(wxT("(DesktopAppBasis::zetLokaliteit) nieuwe taalkode = %d"), static_cast<UInt32>(nieuweTaal));
		wxLogDebug(wxT("(DesktopAppBasis::zetLokaliteit) nieuw taalnaam = ") + taalString);
		wxLogDebug(wxT("(DesktopAppBasis::zetLokaliteit) nieuwe taalkort = ") + taalNaam);
		
		vertaler = new wxTranslations();
		vertaler->SetLanguage(nieuweTaal);
		
		//vertaler->
		wxTranslations::Set(vertaler);
		
		
		taalGekozen = true;
		appConfig.Write(wxT("/Operationeel/EersteKeer/TaalGekozen"), taalGekozen);
		appConfig.Write(wxT("/Gebruikersinstellingen/Encoding"), static_cast<int>(taal = nieuweTaal));
		if (true == slaMeteenOp)
			appConfig.Flush();
		
		retkode = RetCode::Ok;
	}
	
	return(retkode);
}

RetCode DesktopApp::geefLokaliteitStatus() const
{
	const auto retkode = (RetCode::Ok == taalGoedGezet) ? RetCode::Ok : RetCode::Fout;
	return(retkode);
}


int DesktopApp::OnExit()
{
	return(0);
}

wxFileName DesktopApp::geefExePad() const
{
	const wxString exePath(stdPaden.GetExecutablePath());
	const wxFileName pad(exePath);
	return(pad);
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
	const wxString uit(applikatieNaam);
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

