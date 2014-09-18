// GDA03C.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Utility.h"
#include "EntryClass.h"

int _tmain(int argc, _TCHAR* argv[])
{
	unsigned int seed = time(0);
	srand(seed);

	if(!MyLog::init()) {
		return -1;
	}

	EntryClass app;
	app.go();

	return 0;
}

