/*!
@file 
Opdracht 1 DSB practicum. Werk deze opdracht verder uit aan de hand van het kommentaar.
Assignment 1 DSP. Complete this assignment guided by the commentary.
@version $Rev: 316 $
@author $Author: ewout $ 
                                   
@copyright Copyright 2006-2022 ir drs E.J Boks, Hogeschool van Arnhem en Nijmegen. https://ese.han.nl
  
$URL: https://ese.han.nl/svn/dsbpracticum/trunk/2022/software/opdracht1/Basis/applikatie1.cpp $  
$Id: applikatie1.cpp 316 2023-02-17 09:58:44Z ewout $
************************************************************************/

#include <dsbRingBuffer.h>
#include <dsbComplex.h>
#include <applikatie1.h>

#include <wx/txtstrm.h>
#include <complex>
#include <dsbComplexBasis.h>

IMPLEMENT_APP(DSBKlassenApp);

DSBKlassenApp::DSBKlassenApp() : ConsoleApp(wxT("DSBKlassen"))
{
    wxLogDebug(wxT("DSPESE practicum opdracht/assignment 1."));
}

bool DSBKlassenApp::OnInit()
{
	return(true);
}

// TODO nog uitwerken.
#ifdef CLION
#define wxLogDebug wxLogMessage
#endif

int DSBKlassenApp::MainLoop()
{
    Complex a(2,3),b(1,-1.6f);
	std::complex<float> aTest(2,3),bTest(1,-1.6f);

    /* test code */
    using CP = ComplexPrinter;
    using PP = PolairPrinter;

	const auto plus(a + b);
    wxLogDebug(CP::str(a) + wxT("+")+CP::str(b) +wxT(" = ")+ CP::str(plus));
	wxASSERT((plus.Re() == (aTest+bTest).real()) && 
			  plus.Im() == (aTest+bTest).imag());

	const auto min(b-a);
    wxLogDebug(CP::str(b) + wxT("-")+CP::str(a) +wxT(" = ")+ CP::str(min));
	wxASSERT(((min).Re() == (bTest-aTest).real()) && 
			  (min).Im() == (bTest-aTest).imag());

	const auto maal(a*b);
    wxLogDebug(CP::str(a) + wxT("*")+CP::str(b) +wxT(" = ")+ CP::str(maal));
	wxASSERT(((maal).Re() == (aTest*bTest).real()) && 
			  (maal).Im() == (aTest*bTest).imag());


	const auto deling(a / b);
    wxLogDebug(CP::str(a) + wxT("/")+CP::str(b) +wxT(" = ")+ CP::str(deling));
	const std::complex<float> delingControle(aTest/bTest);
	//wxLogDebug(wxString::Format(wxT("Check : a/b = [%.3f,%.3f]"),delingControle.real(),delingControle.imag()));
	wxASSERT((fabsf(deling.Re() - (aTest / bTest).real()) < 1E-3) &&
		(fabsf(deling.Im() - (aTest / bTest).imag()) < 1E-3));

#if defined(InterfaceTaalNederlands)
    wxLogDebug(CP::str(a) + wxT(" in polaire notatie = ")+PP::str(a.polair()));
	wxLogDebug(wxString::Format(wxT("Check : a_pol = |%.3f|/_%.3f"),abs(aTest),arg(aTest)));

    static constexpr auto aantal=7;
    StaticRingBuffer<Complex,aantal> buffer;

    buffer.schrijf(a);
    buffer.schrijf(b);
    
    for (auto i=0;i<2*aantal;i++)
    {
        a+=Complex(4.0f*i,4.0f*i+5);
        buffer.schrijf(a);
        wxLogDebug(wxString::Format(wxT("waarde [%d] geschreven in buffer="),i)+CP::str(a));
    }

    wxLogDebug(wxString::Format(wxT("Buffer teruglezen. De buffer is %d elementen groot."),buffer.geefAantal()));
	const auto bufw1(buffer.lees(2));
	const auto bufw2(buffer.lees(5));

   	wxLogDebug(wxT("Buffer element [n-2] = ")+CP::str(bufw1));
	wxLogDebug(wxT("Buffer element [n-5] = ")+CP::str(bufw2));

	/* controleer dat de waarden kloppen. */
	static constexpr Complex controle1(266.0f,327.0f);
	static constexpr Complex controle2(146.0f,192.0f);

	wxASSERT(controle1 == bufw1);
	wxASSERT(controle2 == bufw2);

	static constexpr Complex controle3(114.0f,155.0f);
	static constexpr Complex controle4(222.0f,278.0f);

	wxLogDebug(wxT("Buffer element [2] = ")+CP::str(buffer[2]));
	wxLogDebug(wxT("Buffer element [5] = ")+CP::str(buffer[5]));

	wxASSERT(controle3 == buffer[2]);
	wxASSERT(controle4 == buffer[5]);

	Complex controle5,controle6;
    for (auto i=0;i<2*aantal;i++)
    {
        const auto gelezen(buffer.lees());
		if (4==i)
			controle5 = gelezen;
		else if (4+aantal == i)
			controle6 = gelezen;
        wxLogDebug(wxString::Format(wxT("buffer[n-%d]="),i)+
                   CP::str(gelezen));
    }

	wxASSERT(controle5 == controle6);

#elif defined (InterfaceTaalEnglish)

	wxLogDebug(CP::str(a) + wxT(" in polar form = ")+PP::str(a.polar()));
	wxLogDebug(wxString::Format(wxT("Check : a_pol = |%.3f|/_%.3f"),abs(aTest),arg(aTest)));

	static constexpr auto bufferSize = 7;
	StaticRingBuffer<Complex,bufferSize> buffer;

	buffer.write(a);
	buffer.write(b);

	for (auto i=0;i<2*buffer.giveSize();i++)
	{
		a+=Complex(4*i,4*i+5);
		buffer.write(a);
		wxLogDebug(wxString::Format(wxT("value [%d] written in buffer="),i)+CP::str(a));
	}

	wxLogDebug(wxString::Format(wxT("Buffer read-back. The buffer has a size of %d elements."), buffer.giveSize()));
	const auto bufw1(buffer.read(2));
	const auto bufw2(buffer.read(5));

   	wxLogDebug(wxT("Buffer element [n-2] = ")+CP::str(bufw1));
	wxLogDebug(wxT("Buffer element [n-5] = ")+CP::str(bufw2));

	/* verify that the values are correct. */
	static constexpr Complex controle1(266.0f,327.0f);
	static constexpr Complex controle2(146.0f,192.0f);

	wxASSERT(controle1 == bufw1);
	wxASSERT(controle2 == bufw2);

	static constexpr Complex controle3(114.0f,155.0f);
	static constexpr Complex controle4(222.0f,278.0f);

	wxLogDebug(wxT("Buffer element [2] = ")+CP::str(buffer[2]));
	wxLogDebug(wxT("Buffer element [5] = ")+CP::str(buffer[5]));

	wxASSERT(controle3 == buffer[2]);
	wxASSERT(controle4 == buffer[5]);

	Complex controle5,controle6;

	for (auto i=0;i<2*buffer.giveSize();i++)
	{
		const auto readElement(buffer.read());
		if (4==i)
			controle5 = readElement;
		else if (4+bufferSize == i)
			controle6 = readElement;

		wxLogDebug(wxString::Format(wxT("buffer[n-%d]="),i)+
		           CP::str(readElement));
	}

	wxASSERT(controle5 == controle6);

#else
#error "Geen taal gedefineerd in DSB.cmake / No language was defined in DSB.cmake ."
#endif
    return(0);
}

