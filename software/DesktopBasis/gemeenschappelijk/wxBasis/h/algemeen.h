/*!
@file
Desktop algemeen API voor HAN ESE applikaties.

@version $Rev: 4685 $
@author $Author: ewout $
@copyright Copyright 2006-2019 ir drs E.J Boks Hogeschool van Arnhem en Nijmegen
$Id: algemeen.h 4685 2023-03-08 15:50:32Z ewout $
*/

#ifndef DESKTOPAlgemeen_H
#define DESKTOPAlgemeen_H

class WiskundeUitdrukkingParser
{
public:
	WiskundeUitdrukkingParser() = default;
	
	double evalueer(const wxString &);
	
	bool goed() const;
	double laatsteWaarde() const;

private:
	
	int fout=0;
	float resultaat=0.0f;
	bool laatsteGoed=false;
};

#endif
