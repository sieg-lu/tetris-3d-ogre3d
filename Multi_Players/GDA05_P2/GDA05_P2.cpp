// GDA05_P2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Utility.h"
#include "EntryClass.h"

int _tmain(int argc, _TCHAR* argv[])
{
	std::cout << "input the server ip address: \n";
	gets(gServerIp);
	if (gServerIp[0] == '\0') {
		strcpy(gServerIp, "127.0.0.1");
	}

	if(!MyLog::init()) {
		return -1;
	}

	EntryClass app;
	app.go();

	return 0;
}

