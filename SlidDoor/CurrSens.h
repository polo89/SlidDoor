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
	int _sampleRate;
	double _offset = 2500.0; // mV (ACS712T)
	const double _mVperA = 185.0; // mV/A (ACS712T)
public:
	CurrSens(int pin, int sampleRate);
	double Read();
	void Calibrate();
};

#endif

