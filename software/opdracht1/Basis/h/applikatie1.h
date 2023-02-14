#ifndef MAIN_H
#define MAIN_H

#include <desktopApp.h>

class DSBKlassenApp : public ConsoleApp
{
public:
	DSBKlassenApp();

	bool OnInit();

private:
	int MainLoop();
	
};


#endif
