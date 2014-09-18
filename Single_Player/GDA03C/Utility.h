#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include "TinyXml/tinyxml.h"

#define MAX_X 5
#define MAX_Y 12
#define MAX_Z 5

enum CubeType
{
	eNone = 0,
	eGeneral,
	eExplosive,
	eExtendable,
};

static class MyLog
{
public:
	static bool init()
	{
		std::ofstream fout = std::ofstream("GDA03_LOG_FILE.txt");
		if (!fout) {
			return false;
		}
		fout.close();
		return true;
	}

	static void put(int line, char* file, char* msg)
	{
		std::ofstream fout("GDA03_LOG_FILE.txt", std::ios::app);
//		std::cout << "Line " << line << " in file: " << file << " --" << msg << std::endl;
		fout << "Line " << line << " in file: " << file << " --" << msg << std::endl;
		fout.close();
	}
};

class CustomTimer
{
public:
	CustomTimer()
		: _startTime(clock())
	{

	}

	void Start()
	{
		_startTime = clock();
	}

	int CurrentTime()
	{
		return clock() - _startTime;
	}

private:
	clock_t _startTime;
};

static class Utility
{
public:
	static bool isXPosLegal(int x)		{ return (x >= 0 && x <= MAX_X); }

	static bool isYPosLegal(int y)		{ return (y >= 0 && y <= MAX_Y); }

	static bool isZPosLegal(int z)		{ return (z >= 0 && z <= MAX_Z); }

};

#endif