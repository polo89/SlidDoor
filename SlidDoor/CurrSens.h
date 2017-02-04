#ifndef _CURRSENS_h
#define _CURRSENS_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class CurrSens
{
private:
	int _pin;
	const int _offset = 2500; // mV (ACS712T)
	const int _mVperA = 185; // mV/A (ACS712T)
public:
	CurrSens(int pin);
	double Read();
};

#endif

